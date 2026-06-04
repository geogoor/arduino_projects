package com.weather.dashboard

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
import com.weather.dashboard.databinding.ActivityMainBinding
import java.io.IOException
import java.util.UUID

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private var bluetoothAdapter: BluetoothAdapter? = null
    private var bluetoothSocket: BluetoothSocket? = null
    private var isConnected = false

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
    }

    // --- Permissions ---

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
        requestCode: Int, permissions: Array<out String>, grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQUEST_BT_PERMISSION &&
            grantResults.firstOrNull() == PackageManager.PERMISSION_GRANTED
        ) {
            showDeviceSelector()
        } else {
            Toast.makeText(this, "Bluetooth permission required", Toast.LENGTH_SHORT).show()
        }
    }

    // --- Device Selector ---

    @SuppressLint("MissingPermission")
    private fun showDeviceSelector() {
        val paired: Set<BluetoothDevice> = bluetoothAdapter?.bondedDevices ?: emptySet()
        if (paired.isEmpty()) {
            Toast.makeText(this, "No paired devices.\nPair the HC-05 from Settings -> Bluetooth", Toast.LENGTH_LONG).show()
            return
        }

        val names    = paired.map { "${it.name}  (${it.address})" }.toTypedArray()
        val devices  = paired.toList()

        AlertDialog.Builder(this)
            .setTitle("Select HC-05 / HC-06")
            .setItems(names) { _, i -> connectToDevice(devices[i]) }
            .setNegativeButton("Cancel", null)
            .show()
    }

    // --- Bluetooth Connect ---

    @SuppressLint("MissingPermission")
    private fun connectToDevice(device: BluetoothDevice) {
        binding.tvStatus.text  = "Connecting to ${device.name}..."
        binding.btnConnect.isEnabled = false

        Thread {
            try {
                bluetoothAdapter?.cancelDiscovery()
                bluetoothSocket = device.createRfcommSocketToServiceRecord(SPP_UUID)
                bluetoothSocket!!.connect()
                isConnected = true

                runOnUiThread {
                    binding.tvStatus.text     = "● ${device.name}"
                    binding.btnConnect.text   = "Disconnect"
                    binding.btnConnect.isEnabled = true
                }
                startReading()

            } catch (e: IOException) {
                isConnected = false
                runOnUiThread {
                    binding.tvStatus.text     = "Connection failed"
                    binding.btnConnect.text   = "Connect"
                    binding.btnConnect.isEnabled = true
                    Toast.makeText(this, "Failed: ${e.message}", Toast.LENGTH_SHORT).show()
                }
            }
        }.start()
    }

    // --- Read Loop ---

    private fun startReading() {
        Thread {
            val buf    = StringBuilder()
            val bytes  = ByteArray(256)
            val stream = bluetoothSocket!!.inputStream

            while (isConnected) {
                try {
                    val n = stream.read(bytes)
                    buf.append(String(bytes, 0, n))

                    // Parse all complete $W,...# messages
                    var s = buf.indexOf("\$W,")
                    var e = buf.indexOf("#")
                    while (s != -1 && e != -1 && s < e) {
                        val msg = buf.substring(s + 3, e)
                        runOnUiThread { updateUI(msg) }
                        buf.delete(0, e + 1)
                        s = buf.indexOf("\$W,")
                        e = buf.indexOf("#")
                    }

                    if (buf.length > 512) buf.clear() // anti-overflow

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

    // --- Parse & Update UI ---

    private fun updateUI(msg: String) {
        // msg = "23.5,74.3,78,SUNNY,25.1,18.2,3600,0"
        val p = msg.trim().split(",")
        if (p.size < 8) return

        try {
            binding.tvTempC.text  = "${p[0]} °C"
            binding.tvTempF.text  = "${p[1]} °F"
            binding.tvLight.text  = "${p[2]}%"
            binding.tvCond.text   = p[3]
            binding.tvMax.text    = "${p[4]} °C"
            binding.tvMin.text    = "${p[5]} °C"
            binding.tvUptime.text = "Uptime: ${p[6]}s"

            when (p[7].trim()) {
                "1" -> showAlert("⚠  HIGH TEMPERATURE", 0xFFB71C1C.toInt())
                "2" -> showAlert("⚠  LOW TEMPERATURE", 0xFF0D47A1.toInt())
                else -> binding.cardAlert.visibility = View.GONE
            }
        } catch (_: Exception) { }
    }

    private fun showAlert(text: String, color: Int) {
        binding.cardAlert.visibility = View.VISIBLE
        binding.tvAlert.text = text
        binding.cardAlert.setCardBackgroundColor(color)
    }

    // --- Disconnect ---

    private fun disconnect() {
        isConnected = false
        try { bluetoothSocket?.close() } catch (_: IOException) { }
        bluetoothSocket = null
        binding.tvStatus.text   = "Disconnected"
        binding.btnConnect.text = "Connect"
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
