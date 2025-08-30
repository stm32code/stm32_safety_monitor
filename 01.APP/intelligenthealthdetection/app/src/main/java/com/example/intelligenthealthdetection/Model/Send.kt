package com.example.intelligenthealthdetection.Model

data class Send(
    var cmd: Int,
    var heartv: Int? = null,
    var spo2v: Int? = null,
    var tempv: Int? = null

)
