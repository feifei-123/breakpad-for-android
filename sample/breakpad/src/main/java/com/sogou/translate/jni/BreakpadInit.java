package com.sogou.translate.jni;


public class BreakpadInit {
    public static final String TAG = BreakpadInit.class.getSimpleName();
    static {
        System.loadLibrary("breakpad");
    }

    public static void initBreakpad(String path, InfoHelper infoHelper){
        initBreakpadNative(path,infoHelper);
    }

    public static void tryCrash(){
        go2crash();
    }
    public native  static void initBreakpadNative(String path,InfoHelper infoHelper);
    

    public native static void go2crash();
}

