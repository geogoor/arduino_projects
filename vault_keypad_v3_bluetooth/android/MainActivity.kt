package com.vault.keypad

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.vault.keypad.databinding.ActivityMainBinding
import java.io.IOException
import java.io.OutputStream
import java.util.UUID

/**
 * Vault Keypad v3 - secure phone controller.
 *
 * The PIN NEVER leaves the phone. To unlock we use a challenge-response:
 *   app  -> "UNLOCK"
 *   lock -> "$V,CHALLENGE:<nonce>#"
 *   app  -> "RESP:<fnv16( fnv16(PIN) + ":" + nonce )>"
 *   lock -> "$V,UNLOCKED#" / "$V,DENIED:n#" / "$V,LOCKOUT:s#"
 *
 * Same FNV-1a 16-bit maths as the Arduino, so the two sides agree
 * on the response without ever exchanging the PIN itself.
 */
class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private var bluetoothAdapter: BluetoothAdapter? = null
    private var bluetoothSocket: BluetoothSocket? = null
    private var outStream: OutputStream? = null
    private var isConnected = false

    private val entered = StringBuilder()   // digits typed on screen
    private var pendingPin: String? = null  // PIN waiting for a challenge
    private var pendingSet = false          // a "set PIN" flow is in progress

    // SPP UUID - standard for HC-05/HC-06 serial
    private val SPP_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val btManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = btManager.adapter

        binding.btnConnect.setOnClickListener {
            if (isConnected) disconnect() else checkPermissionAndConnect()
        }

        val numButtons = listOf(
            binding.btn0, binding.btn1, binding.btn2, binding.btn3, binding.btn4,
            binding.btn5, binding.btn6, binding.btn7, binding.btn8, binding.btn9
        )
        numButtons.forEachIndexed { digit, b ->
            b.setOnClickListener { onDigit(digit.toString()) }
        }

        // Unlock: start the challenge-response with the typed PIN
        binding.btnUnlock.setOnClickListener {
            requirePin()?.let { pin -> pendingPin = pin; pendingSet = false; send("UNLOCK"); clearEntry() }
        }
        // Set PIN: only the HASH is sent, and only while unlocked
        binding.btnSetPin.setOnClickListener {
            requirePin()?.let { pin -> send("SETHASH:${fnv16(pin)}"); clearEntry() }
        }
        binding.btnLock.setOnClickListener { send("LOCK") }
        binding.btnClear.setOnClickListener { clearEntry() }
    }

    // --- FNV-1a 16-bit, identical to the Arduino ---

    private fun fnv16(s: String): Int {
        var h = 0x811C
        for (c in s) {
            h = h xor (c.code and 0xFF)
            h = (h * 0x0101) and 0xFFFF
        }
        return h and 0xFFFF
    }

    private fun response(pin: String, nonce: String): Int {
        // fnv16( fnv16(PIN) + ":" + nonce )
        return fnv16("${fnv16(pin)}:$nonce")
    }

    // --- On-screen PIN entry ---

    private fun onDigit(d: String) {
        if (entered.length < 4) { entered.append(d); renderEntry() }
    }

    private fun clearEntry() { entered.setLength(0); renderEntry() }

    private fun renderEntry() {
        binding.tvEntry.text = "•".repeat(entered.length).padEnd(4, '_')
    }

    private fun requirePin(): String? {
        if (entered.length != 4) {
            Toast.makeText(this, "Enter 4 digits first", Toast.LENGTH_SHORT).show()
            return null
        }
        return entered.toString()
    }

    // --- Send a command line to the Arduino ---

    private fun send(cmd: String) {
        if (!isConnected) { Toast.makeText(this, "Not connected", Toast.LENGTH_SHORT).show(); return }
        Thread {
            try {
                outStream?.write((cmd + "\n").toByteArray())
            } catch (e: IOException) {
                runOnUiThread { Toast.makeText(this, "Send failed", Toast.LENGTH_SHORT).show() }
            }
        }.start()
    }

    // --- Permissions ---

    private fun checkPermissionAndConnect() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
                != PackageManager.PERMISSION_GRANTED
            ) {
                ActivityCompat.requestPermissions(
                    this, arrayOf(Manifest.permission.BLUETOOTH_CONNECT), REQUEST_BT_PERMISSION
                )
                return
            }
        }
        showDeviceSelector()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int, permissions: Array<out String>, grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_BT_PERMISSION &&
            grantResults.firstOrNull() == PackageManager.PERMISSION_GRANTED
        ) showDeviceSelector()
        else Toast.makeText(this, "Bluetooth permission required", Toast.LENGTH_SHORT).show()
    }

    // --- Device Selector ---

    @SuppressLint("MissingPermission")
    private fun showDeviceSelector() {
        val paired: Set<BluetoothDevice> = bluetoothAdapter?.bondedDevices ?: emptySet()
        if (paired.isEmpty()) {
            Toast.makeText(this, "No paired devices.\nPair the HC-05 from Settings -> Bluetooth", Toast.LENGTH_LONG).show()
            return
        }
        val names   = paired.map { "${it.name}  (${it.address})" }.toTypedArray()
        val devices = paired.toList()
        AlertDialog.Builder(this)
            .setTitle("Select HC-05 / HC-06")
            .setItems(names) { _, i -> connectToDevice(devices[i]) }
            .setNegativeButton("Cancel", null)
            .show()
    }

    // --- Bluetooth Connect ---

    @SuppressLint("MissingPermission")
    private fun connectToDevice(device: BluetoothDevice) {
        binding.tvStatus.text = "Connecting to ${device.name}..."
        binding.btnConnect.isEnabled = false
        Thread {
            try {
                bluetoothAdapter?.cancelDiscovery()
                bluetoothSocket = device.createRfcommSocketToServiceRecord(SPP_UUID)
                bluetoothSocket!!.connect()
                outStream = bluetoothSocket!!.outputStream
                isConnected = true
                runOnUiThread {
                    binding.tvStatus.text = "● ${device.name}"
                    binding.btnConnect.text = "Disconnect"
                    binding.btnConnect.isEnabled = true
                }
                startReading()
            } catch (e: IOException) {
                isConnected = false
                runOnUiThread {
                    binding.tvStatus.text = "Connection failed"
                    binding.btnConnect.text = "Connect"
                    binding.btnConnect.isEnabled = true
                    Toast.makeText(this, "Failed: ${e.message}", Toast.LENGTH_SHORT).show()
                }
            }
        }.start()
    }

    // --- Read Loop (status frames: $V,...#) ---

    private fun startReading() {
        Thread {
            val buf = StringBuilder()
            val bytes = ByteArray(256)
            val stream = bluetoothSocket!!.inputStream
            while (isConnected) {
                try {
                    val n = stream.read(bytes)
                    buf.append(String(bytes, 0, n))
                    var s = buf.indexOf("\$V,")
                    var e = buf.indexOf("#")
                    while (s != -1 && e != -1 && s < e) {
                        val msg = buf.substring(s + 3, e)
                        runOnUiThread { onStatus(msg) }
                        buf.delete(0, e + 1)
                        s = buf.indexOf("\$V,")
                        e = buf.indexOf("#")
                    }
                    if (buf.length > 512) buf.clear()
                } catch (ex: IOException) { break }
            }
            runOnUiThread {
                binding.tvStatus.text = "Disconnected"
                binding.btnConnect.text = "Connect"
                isConnected = false
            }
        }.start()
    }

    // --- Render status coming back from the lock ---

    private fun onStatus(raw: String) {
        val msg = raw.trim()
        when {
            msg.startsWith("CHALLENGE:") -> {
                // answer with the response derived from the pending PIN
                val nonce = msg.substringAfter("CHALLENGE:")
                val pin = pendingPin
                pendingPin = null
                if (pin != null) send("RESP:${response(pin, nonce)}")
            }
            msg == "LOCKED"   -> setState("LOCKED", 0xFFB71C1C.toInt())
            msg == "UNLOCKED" -> setState("UNLOCKED", 0xFF2E7D32.toInt())
            msg.startsWith("DENIED:") -> {
                binding.tvResult.text = "Access denied (${msg.substringAfter(":")}/3)"; flashResult()
            }
            msg.startsWith("LOCKOUT:") -> {
                binding.tvResult.text = "Locked out — wait ${msg.substringAfter(":")}s"; flashResult()
            }
            msg == "PIN_SET"     -> binding.tvResult.text = "PIN updated"
            msg == "NEED_UNLOCK" -> binding.tvResult.text = "Unlock first to change PIN"
            msg == "NO_CHALLENGE"-> binding.tvResult.text = "Tap Open to start"
            msg == "ALERT:DURESS"-> binding.tvResult.text = "⚠ Duress unlock"
            msg == "READY"       -> binding.tvResult.text = "Lock ready"
            else -> binding.tvResult.text = msg
        }
    }

    private fun setState(text: String, color: Int) {
        binding.tvState.text = text
        binding.cardState.setCardBackgroundColor(color)
    }

    private fun flashResult() {
        binding.tvResult.alpha = 0.2f
        binding.tvResult.animate().alpha(1f).setDuration(400).start()
    }

    // --- Disconnect ---

    private fun disconnect() {
        isConnected = false
        try { bluetoothSocket?.close() } catch (_: IOException) { }
        bluetoothSocket = null
        outStream = null
        binding.tvStatus.text = "Disconnected"
        binding.btnConnect.text = "Connect"
    }

    override fun onDestroy() {
        super.onDestroy()
        disconnect()
    }

    companion object {
        private const val REQUEST_BT_PERMISSION = 1
    }
}
