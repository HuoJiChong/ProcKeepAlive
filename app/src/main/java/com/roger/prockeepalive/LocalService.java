package com.roger.prockeepalive;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Binder;
import android.os.IBinder;
import android.os.LocaleList;
import android.os.RemoteException;
import android.support.annotation.Nullable;
import android.telecom.ConnectionService;
import android.util.Log;

/**
 * 本地服务
 * Created by J.C. on 23/03/2019.
 */

public class LocalService extends Service {

    private static final String TAG = "LocalService";

    MyConn conn;
    MyBinder binder;

    @Override
    public IBinder onBind(@Nullable Intent intent) {
        return binder;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        binder = new MyBinder();
        if (conn == null){
            conn = new MyConn();
        }
        Log.e(TAG,"onCreate");
    }

    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
        Log.e(TAG,"onStart");
        this.bindService(new Intent(this,RemoteService.class),conn, Context.BIND_IMPORTANT);
    }

    class MyBinder extends ProcessServiceInterface.Stub{

        @Override
        public String getServiceName() throws RemoteException {
            return "LocalService";
        }
    }

    class MyConn implements ServiceConnection{

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.e(TAG,"onServiceConnected");
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Log.e(TAG,"onServiceDisconnected");
            LocalService.this.unbindService(conn);

            LocalService.this.startService(new Intent(LocalService.this,RemoteService.class));

            LocalService.this.bindService(new Intent(LocalService.this,RemoteService.class),conn, Context.BIND_IMPORTANT);
        }
    }

}
