package com.example.mondi.robotcontrol

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.util.JsonReader
import com.android.volley.Request
import com.android.volley.Response
import com.android.volley.toolbox.StringRequest
import com.android.volley.toolbox.Volley
import com.anychart.anychart.*
import com.anychart.anychart.Set
import kotlinx.android.synthetic.main.activity_chart.*
import org.jetbrains.anko.toast
import java.io.InputStream
import java.util.*

class ChartActivity : AppCompatActivity() {

    val random = Random()
    fun rand(from: Int, to: Int) : Int {
        return random.nextInt(to - from) + from
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_chart)

        val cartesian = AnyChart.line()

        cartesian.setAnimation(true)

        cartesian.setPadding(10.0, 20.0, 5.0, 20.0)

        cartesian.crosshair.setEnabled(true)
        cartesian.crosshair
                .setYLabel(true)
                .setYStroke(null as Stroke?, null, null, null, null)

        cartesian.tooltip.setPositionMode(TooltipPositionMode.POINT)

        val dt = Date()
        cartesian.setTitle("Température et Humidité récupéré par le robot.")

        cartesian.yAxis.setTitle(dt.toLocaleString())
        cartesian.xAxis.labels.setPadding(5.0, 5.0, 5.0, 5.0)

        val seriesData = ArrayList<DataEntry>()

        val handler = Handler()
        val delay = 5000 //milliseconds


        handler.postDelayed(object : Runnable {
            override fun run() {

                val queue = Volley.newRequestQueue(this@ChartActivity)
                val url = "http://xxxxx.xxxx/robot/chart_json.php"
                val postRequest = object : StringRequest(Request.Method.GET, url,
                        Response.Listener { response ->

                            seriesData.clear()

                            val dht = response.splitToSequence(';').toList().toTypedArray()

                            var hm = ""
                            var tem = ""
                            var hum = ""

                            for(ht: String in dht)
                            {
                                val tmp = ht.splitToSequence('|').toList().toTypedArray()
                                for(t: String in tmp)
                                    if(t != "")
                                        if(hm == "")
                                            hm = t
                                        else if(tem == "")
                                            tem = t
                                        else if(hum == "")
                                            hum = t

                                //toast(rand(25,28).toString())




                            }

                            seriesData.add(CustomDataEntry(hm, tem.toDouble(), hum.toDouble()))

                            val set = Set(seriesData)
                            val series1Mapping = set.mapAs("{ x: 'x', value: 'value' }")
                            val series2Mapping = set.mapAs("{ x: 'x', value: 'value2' }")

                            val series1 = cartesian.line(series1Mapping)
                            series1.setName("Température")
                            series1.hovered.markers.setEnabled(true)
                            series1.hovered.markers
                                    .setType(MarkerType.CIRCLE)
                                    .setSize(4.0)
                            series1.tooltip
                                    .setPosition("right")
                                    .setAnchor(EnumsAnchor.LEFT_CENTER)
                                    .setOffsetX(5.0)
                                    .setOffsetY(5.0)

                            val series2 = cartesian.line(series2Mapping)
                            series2.setName("Humidité")
                            series2.hovered.markers.setEnabled(true)
                            series2.hovered.markers
                                    .setType(MarkerType.CIRCLE)
                                    .setSize(4.0)
                            series2.tooltip
                                    .setPosition("right")
                                    .setAnchor(EnumsAnchor.LEFT_CENTER)
                                    .setOffsetX(5.0)
                                    .setOffsetY(5.0)

                            cartesian.legend.setEnabled(true)
                            cartesian.legend.setFontSize(13.0)
                            cartesian.legend.setPadding(0.0, 0.0, 10.0, 0.0)

                            any_chart_view.setChart(cartesian)

                        },
                        Response.ErrorListener { req ->

                        }
                ) {

                }
                queue.add(postRequest)

                //handler.postDelayed(this, delay.toLong())
            }
        }, delay.toLong())


        val set = Set(seriesData)
        val series1Mapping = set.mapAs("{ x: 'x', value: 'value' }")
        val series2Mapping = set.mapAs("{ x: 'x', value: 'value2' }")

        val series1 = cartesian.line(series1Mapping)
        series1.setName("Température")
        series1.hovered.markers.setEnabled(true)
        series1.hovered.markers
                .setType(MarkerType.CIRCLE)
                .setSize(4.0)
        series1.tooltip
                .setPosition("right")
                .setAnchor(EnumsAnchor.LEFT_CENTER)
                .setOffsetX(5.0)
                .setOffsetY(5.0)

        val series2 = cartesian.line(series2Mapping)
        series2.setName("Humidité")
        series2.hovered.markers.setEnabled(true)
        series2.hovered.markers
                .setType(MarkerType.CIRCLE)
                .setSize(4.0)
        series2.tooltip
                .setPosition("right")
                .setAnchor(EnumsAnchor.LEFT_CENTER)
                .setOffsetX(5.0)
                .setOffsetY(5.0)

        cartesian.legend.setEnabled(true)
        cartesian.legend.setFontSize(13.0)
        cartesian.legend.setPadding(0.0, 0.0, 10.0, 0.0)

        any_chart_view.setChart(cartesian)

    }



    private inner class CustomDataEntry internal constructor(x: String, value: Number, value2: Number) : ValueDataEntry(x, value) {

        init {
            setValue("value2", value2)
        }

    }
}
