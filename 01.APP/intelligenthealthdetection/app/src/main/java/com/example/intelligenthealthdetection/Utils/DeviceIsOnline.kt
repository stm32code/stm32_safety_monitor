package com.example.intelligentwaterheater.Utils

import com.example.intelligentwaterheater.Utils.Common.DeviceOnline
import com.example.intelligentwaterheater.Utils.Common.Drive2ID
import com.example.intelligentwaterheater.Utils.Common.api_key
import okhttp3.*
import org.json.JSONObject
import java.io.IOException

class DeviceIsOnline {

    /**
     * 获取网络数据--
     */
    fun getOnline() {
        val okHttp = OkHttpClient()
        val request = Request.Builder()
            .url(
                "https://api.heclouds.com/devices/$Drive2ID"
            )
            .addHeader(
                "api-key", api_key
            )
            .build()
        val call = okHttp.newCall(request)
        call.enqueue(object : Callback {
            @Throws(IOException::class)
            override fun onResponse(call: Call, response: Response) {
                DeviceOnline = try {
                    val responseData = response.body()!!.string()
                    val root = JSONObject(responseData)
                    val data = root.getJSONObject("data")
                    println(data)
                    data.getBoolean("online")
                } catch (e: IOException) {
                    e.printStackTrace()
                    false
                }
            }
            override fun onFailure(call: Call, e: IOException) {
                DeviceOnline = false
            }
        })
    }
}