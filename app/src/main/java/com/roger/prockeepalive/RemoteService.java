package com.roger.prockeepalive;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.support.annotation.Nullable;
import android.util.Log;

/**
 * 远程服务
 * Created by J.C. on 23/03/2019.
 */

public class RemoteService extends Service {

    private static final String TAG = "RemoteService";

    private MyConn conn;
    private MyBinder binder;

    @Override
    public IBinder onBind(@Nullable Intent intent) {
        return binder;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.e(TAG,"onCreate");
        binder = new MyBinder();
        if (conn == null){
            conn = new MyConn();
        }
    }

    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
        Log.e(TAG,"onStart");
        this.bindService(new Intent(this,LocalService.class),conn,Context.BIND_IMPORTANT);
    }

    class MyBinder extends ProcessServiceInterface.Stub{

        @Override
        public String getServiceName() throws RemoteException {
            return "RemoteService";
        }
    }

//    Service Service has leaked ServiceConnection Service$MyConn@a5e16840 that was originally bound here
    class MyConn implements ServiceConnection {

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.e(TAG,"onServiceConnected");
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Log.e(TAG,"onServiceDisconnected");
            RemoteService.this.unbindService(conn);

            RemoteService.this.startService(new Intent(RemoteService.this,LocalService.class));

            RemoteService.this.bindService(new Intent(RemoteService.this,LocalService.class),conn, Context.BIND_IMPORTANT);
        }
    }
}
