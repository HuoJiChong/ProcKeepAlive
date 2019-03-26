package com.aly.roger.socketkeep;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Process;
import android.util.Log;

import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by J.C. on 23/03/2019.
 */

public class ProcService extends Service {

    static {
        System.loadLibrary("Watcher");
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    int i = 0;


    @Override
    public void onCreate() {
        super.onCreate();
        Watcher watcher = new Watcher();
        watcher.createWatcher(String.valueOf(Process.myPid()));
        watcher.createMonitor();

        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                Log.e("Proc"," 守护服务 " + i++);
            }
        },0,2000);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
