package com.motion.security

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
import android.view.View
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.motion.security.databinding.ActivityMainBinding
import java.io.IOException
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.UUID

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private var bluetoothAdapter: BluetoothAdapter? = null
    private var bluetoothSocket: BluetoothSocket? = null
    private var isConnected = false

    private val log = StringBuilder()
    private val ts = SimpleDateFormat("HH:mm:ss", Locale.getDefault())

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
    }

    private fun checkPermissionAndConnect() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT)
                != PackageManager.PERMISSION_GRANTED
            ) {
                ActivityCompat.requestPermissions(
                    this,
                    arrayOf(Manifest.permission.BLUETOOTH_CONNECT),
                    REQUEST_BT_PERMISSION
                )
                return
            }
        }
        showDeviceSelector()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_BT_PERMISSION &&
            grantResults.firstOrNull() == PackageManager.PERMISSION_GRANTED
        ) {
            showDeviceSelector()
        } else {
            Toast.makeText(this, "Bluetooth permission απαιτείται", Toast.LENGTH_SHORT).show()
        }
    }

    @SuppressLint("MissingPermission")
    private fun showDeviceSelector() {
        val paired: Set<BluetoothDevice> = bluetoothAdapter?.bondedDevices ?: emptySet()
        if (paired.isEmpty()) {
            Toast.makeText(
                this,
                "Δεν υπάρχουν paired devices.\nΚάνε pair το HC-05 από Settings → Bluetooth",
                Toast.LENGTH_LONG
            ).show()
            return
        }

        val names   = paired.map { "${it.name}  (${it.address})" }.toTypedArray()
        val devices = paired.toList()

        AlertDialog.Builder(this)
            .setTitle("Επίλεξε HC-05 / HC-06")
            .setItems(names) { _, i -> connectToDevice(devices[i]) }
            .setNegativeButton("Άκυρο", null)
            .show()
    }

    @SuppressLint("MissingPermission")
    private fun connectToDevice(device: BluetoothDevice) {
        binding.tvStatus.text        = "Connecting to ${device.name}..."
        binding.btnConnect.isEnabled = false

        Thread {
            try {
                bluetoothAdapter?.cancelDiscovery()
                bluetoothSocket = device.createRfcommSocketToServiceRecord(SPP_UUID)
                bluetoothSocket!!.connect()
                isConnected = true

                runOnUiThread {
                    binding.tvStatus.text        = "● ${device.name}"
                    binding.btnConnect.text      = "Disconnect"
                    binding.btnConnect.isEnabled = true
                    addLog("connected — waiting for events")
                }
                startReading()

            } catch (e: IOException) {
                isConnected = false
                runOnUiThread {
                    binding.tvStatus.text        = "Connection failed — δοκίμασε 38400 baud"
                    binding.btnConnect.text      = "Connect"
                    binding.btnConnect.isEnabled = true
                    Toast.makeText(this, "Αποτυχία: ${e.message}", Toast.LENGTH_SHORT).show()
                }
            }
        }.start()
    }

    private fun startReading() {
        Thread {
            val buf   = StringBuilder()
            val bytes = ByteArray(256)
            val stream = bluetoothSocket!!.inputStream

            while (isConnected) {
                try {
                    val n = stream.read(bytes)
                    buf.append(String(bytes, 0, n))

                    // frames look like  $STATUS,ARMED#  or  $ALERT,INTRUSION#
                    var s = buf.indexOf("\$")
                    var e = buf.indexOf("#")
                    while (s != -1 && e != -1 && s < e) {
                        val msg = buf.substring(s + 1, e)
                        runOnUiThread { handleMessage(msg) }
                        buf.delete(0, e + 1)
                        s = buf.indexOf("\$")
                        e = buf.indexOf("#")
                    }

                    if (buf.length > 512) buf.clear()

                } catch (ex: IOException) {
                    break
                }
            }

            runOnUiThread {
                binding.tvStatus.text   = "Disconnected"
                binding.btnConnect.text = "Connect"
                isConnected = false
            }
        }.start()
    }

    private fun handleMessage(msg: String) {
        val p = msg.trim().split(",")
        if (p.size < 2) return
        when (p[0]) {
            "STATUS" -> setState(p[1])
            "ALERT"  -> if (p[1] == "INTRUSION") showIntrusion()
        }
        addLog(msg)
    }

    private fun setState(state: String) {
        binding.cardAlert.visibility = View.GONE
        when (state) {
            "DISARMED" -> setStateCard("DISARMED", "system is off", 0xFF37474F.toInt())
            "ARMING"   -> setStateCard("ARMING…", "exit delay, leave the area", 0xFFF9A825.toInt())
            "ARMED"    -> setStateCard("ARMED", "secured", 0xFF2E7D32.toInt())
            "ENTRY"    -> setStateCard("ENTRY", "enter PIN to disarm", 0xFFEF6C00.toInt())
            else       -> setStateCard(state, "", 0xFF37474F.toInt())
        }
    }

    private fun setStateCard(title: String, sub: String, color: Int) {
        binding.tvState.text    = title
        binding.tvStateSub.text = sub
        binding.cardState.setCardBackgroundColor(color)
    }

    private fun showIntrusion() {
        setStateCard("INTRUSION", "alarm triggered", 0xFFB71C1C.toInt())
        binding.cardAlert.visibility = View.VISIBLE
        binding.tvAlert.text = "🚨  ΕΙΣΒΟΛΗ ΑΝΙΧΝΕΥΘΗΚΕ"
    }

    private fun addLog(line: String) {
        log.insert(0, "${ts.format(Date())}  $line\n")
        if (log.length > 2000) log.setLength(2000)
        binding.tvLog.text = log.toString()
    }

    private fun disconnect() {
        isConnected = false
        try { bluetoothSocket?.close() } catch (_: IOException) { }
        bluetoothSocket = null
        binding.tvStatus.text        = "Disconnected"
        binding.btnConnect.text      = "Connect"
        binding.cardAlert.visibility = View.GONE
    }

    override fun onDestroy() {
        super.onDestroy()
        disconnect()
    }

    companion object {
        private const val REQUEST_BT_PERMISSION = 1
    }
}
