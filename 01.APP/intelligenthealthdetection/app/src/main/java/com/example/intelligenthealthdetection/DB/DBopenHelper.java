package com.example.intelligenthealthdetection.DB;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import androidx.annotation.Nullable;

public class DBopenHelper extends SQLiteOpenHelper {
    final static String DB_Name = "my.db"; //数据库名
    final static int VERSION = 1;//版本号

    public DBopenHelper(@Nullable Context context) {
        super(context, DB_Name, null, VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        //用户表
        db.execSQL(
                "CREATE TABLE `history`(" +
                        "`hid` INTEGER  primary key AUTOINCREMENT," +
                        "`htype` INTEGER," +
                        "`hdata` char(20)," +
                        "`htime` char(20)) "
        );

    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
}
