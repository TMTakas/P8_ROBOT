package com.example.mondi.robotcontrol

import android.content.Intent
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.text.TextWatcher
import android.view.View
import android.widget.SeekBar
import android.widget.Toast
import com.android.volley.Request
import com.android.volley.Response
import com.android.volley.toolbox.StringRequest
import com.android.volley.toolbox.Volley
import kotlinx.android.synthetic.main.activity_main.*
import org.jetbrains.anko.toast
import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.HttpURLConnection
import java.net.URL
import com.android.volley.VolleyError
import com.android.volley.RequestQueue
import org.jetbrains.anko.editText
import android.text.Editable
import com.anychart.anychart.*
import com.anychart.anychart.Set
import java.util.ArrayList


class MainActivity : AppCompatActivity(), SeekBar.OnSeekBarChangeListener {
    var LastClicked = "F"
    var LastValue = 0
    override fun onStart() {
        super.onStart()

        val handler = Handler()
        val delay = 1000 //milliseconds

        handler.postDelayed(object : Runnable {
            override fun run() {

                val queue = Volley.newRequestQueue(this@MainActivity)
                val url = "http://xxxxxxxx.xxx/robot/get_count.php"
                val postRequest = object : StringRequest(Request.Method.GET, url,
                        Response.Listener { response ->
                            TextViewCount.text = response
                        },
                        Response.ErrorListener { req ->
                            // error
                            //toast(req.message.toString())
                        }
                ) {

                }
                queue.add(postRequest)

                handler.postDelayed(this, delay.toLong())
            }
        }, delay.toLong())

        EditTextTime.addTextChangedListener(object : TextWatcher {

            // the user's changes are saved here
            override fun onTextChanged(c: CharSequence, start: Int, before: Int, count: Int) {
                EditTextCommand.setText("M|" + LastClicked + "|" + LastValue.toString() + "|" + EditTextTime.text)
            }

            override fun beforeTextChanged(c: CharSequence, start: Int, count: Int, after: Int) {
                // this space intentionally left blank
            }

            override fun afterTextChanged(c: Editable) {
                // this one too
            }
        })
    }
    override fun onStartTrackingTouch(seekBar: SeekBar?) {

    }

    override fun onStopTrackingTouch(seekBar: SeekBar?) {

    }

    override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        EditTextCommand.setText("M|" + LastClicked + "|" + progress.toString() + "|" + EditTextTime.text)
        LastValue = progress
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        SeekBarMove.setOnSeekBarChangeListener(this)

        ButtonSendCommand.setOnClickListener {
            ButtonSendCommand.isEnabled = false
            SeekBarMove.isEnabled = false
            val queue = Volley.newRequestQueue(this)
            val url = "http://xxxx.xxx/robot/post_order.php"
            val postRequest = object : StringRequest(Request.Method.POST, url,
                    Response.Listener { response ->
                        ButtonSendCommand.isEnabled = true
                        SeekBarMove.isEnabled = true
                        toast(response)
                    },
                    Response.ErrorListener { req ->
                        // error
                        toast(req.message.toString())
                        ButtonSendCommand.isEnabled = true
                    }
            ) {
                override fun getParams(): Map<String, String> {
                    val params = HashMap<String, String>()
                    params.put("order_to_exe", EditTextCommand.text.toString())
                    return params
                }
            }
            queue.add(postRequest)
        }

        ButtonClearCommands.setOnClickListener {
            ButtonClearCommands.isEnabled = false
            val queue = Volley.newRequestQueue(this)
            val url = "http://xxxxxx.xxx/robot/clear_all_commands.php"
            val postRequest = object : StringRequest(Request.Method.GET, url,
                    Response.Listener { response ->
                        ButtonClearCommands.isEnabled = true
                        SeekBarMove.isEnabled = true
                        toast(response)
                    },
                    Response.ErrorListener { req ->
                        // error
                        toast(req.message.toString())
                        ButtonClearCommands.isEnabled = true
                    }
            ) {

            }
            queue.add(postRequest)
        }

        ButtonGoCharts.setOnClickListener {
            val intent = Intent(this, ChartActivity::class.java)
            startActivity(intent)
        }
    }

    fun ButtonClick(btn: View)
    {
        LastClicked = btn.tag.toString()
        EditTextCommand.setText("M|" + LastClicked + "|" + LastValue.toString() + "|" + EditTextTime.text)
    }
}
