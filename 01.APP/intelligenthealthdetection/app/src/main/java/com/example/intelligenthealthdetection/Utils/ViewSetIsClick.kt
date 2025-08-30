package com.example.intelligentwindows.Utils

import android.view.View

object ViewSetIsClick {
    /**
     * 设置控件是否能点击
     */
    fun setViewIsClick(view: View, click: Boolean) {
        view.isClickable = click
        view.isEnabled = click
        view.isFocusable = click
    }
}