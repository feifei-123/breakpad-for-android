package cn.onlinecache.breakpad.demo;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.widget.Button;

import com.sogou.translate.jni.BreakpadInit;
import com.sogou.translate.jni.InfoHelperImpl;

import java.io.File;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import cn.onlinecache.breakpad.NativeBreakpad;

public class MainActivity extends Activity implements View.OnClickListener{

    @BindView(R.id.btnTestNDKCrash)
    Button btnTestNDKCrash;

    private static final int WRITE_EXTERNAL_STORAGE_REQUEST_CODE = 100;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


//        NativeBreakpad.init(Environment.getExternalStorageDirectory().getAbsolutePath());
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                    this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    WRITE_EXTERNAL_STORAGE_REQUEST_CODE);
        } else {
//            initExternalReportPath();
        }
        String path = "/sdcard/crash/";
        File file = new File(path);
        if(!file.exists()){
            file.mkdirs();
        }
        BreakpadInit.initBreakpad(path,new InfoHelperImpl());
    }

    void testNDKCrash(){
//        NativeBreakpad.testNativeCrash();
        BreakpadInit.tryCrash();
    }

    @Override
    public void onClick(View view) {
        testNDKCrash();
    }
}
