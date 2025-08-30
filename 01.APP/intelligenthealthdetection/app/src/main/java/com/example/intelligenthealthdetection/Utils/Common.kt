package com.example.intelligentwaterheater.Utils


import android.content.Context
import android.util.Log
import com.example.intelligenthealthdetection.Model.Send
import com.example.intelligenthealthdetection.Utils.MToast
import com.google.gson.Gson
import com.itfitness.mqttlibrary.MQTTHelper

object Common {
    var Sever =
        "tcp://8.133.197.10:1883" //mqtt连接ip oneNET为183.230.40.39

    //                var Sever = "tcp://192.168.1.20:1883" //mqtt连接ip oneNET为183.230.40.39
    var PORT = "6002"//oneNET为6002
    var ReceiveTopic =  "/broadcast/66/test2"
    var PushTopic =     "/broadcast/66/test1"
    var DriveID =
        "6-6-app"  //mqtt连接id  oneNET为设备id
    var DriveName = "6-6-app"  //设置用户名。跟Client ID不同。用户名可以看做权限等级"  oneNET为产品ID
    var DrivePassword = "6-6-app"// //设置登录密码  oneNET为设备鉴权或apikey
    var Drive2ID = "1147239937"
    var api_key = "HxWbqoHYZN8WVYsBrdSVe=wcjTY="
    var DeviceOnline = false //底层是否在线
    var LatestOnlineDate = "离线" //最近在线时间
    var mqttHelper: MQTTHelper? = null

    /***
     * @brief 包装发送函数，只有建立了连接才发送消息
     */
    fun sendMessage(context: Context, cmd: Int, vararg data: String): String {
        return if (mqttHelper == null || !mqttHelper!!.connected) {
            MToast.mToast(context, "未建立连接")
            ""
        } else {
            try {
                val send = Send(cmd = cmd)
                when (cmd) {
                    1 -> {
                        send.heartv = data[0].toInt()
                        send.spo2v = data[1].toInt()
                        send.tempv = data[2].toInt()
                    }

                }
                val result = Gson().toJson(send)
                mqttHelper!!.publish(PushTopic, result, 1)
                result
            } catch (e: Exception) {
                e.printStackTrace()
                Log.e("发送错误", e.message.toString())
                MToast.mToast(context, "数据发送失败")
                ""
            }
        }
    }
}


