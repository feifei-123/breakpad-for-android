package com.sogou.translate.jni;

import android.util.Log;

public class InfoHelperImpl implements InfoHelper {

    @Override
    public void dumpLogcat() {
        Log.d("feifei","-----dumpLogcat:"+Thread.currentThread().getName());
        ShellUtils.execCommand("logcat -d > /sdcard/logfeifei.log",false);
        Log.d("feifei","-----dumpLogcat finished!");

    }
}
