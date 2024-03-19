package com.mcuhq.simplebluetooth;

import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.os.SystemClock;
import android.os.Build;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;

import android.os.Bundle;
import android.view.View;
import android.widget.*;

import android.graphics.Color;
public class ConnectedThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private final Handler mHandler;
    private final Context mContext;

    private boolean isConnected = true; // Variable to track connection status

    // Constants for identifying connection status
    public static final int CONNECTION_STATUS_CONNECTED = 1;
    public static final int CONNECTION_STATUS_DISCONNECTED = 0;

    // Getter methods for accessing the connection status constants
   /* public static int getConnectedStatus() {
        return CONNECTION_STATUS_CONNECTED;
    }

    public static int getDisconnectedStatus() {
        return CONNECTION_STATUS_DISCONNECTED;
    }*/

    private static final String NOTIFICATION_CHANNEL_ID = "BluetoothNotificationChannel";
    private static final int NOTIFICATION_ID = 1;

    public ConnectedThread(BluetoothSocket socket, Handler handler, Context context) {
        mmSocket = socket;
        mHandler = handler;
        mContext = context;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        // Get the input and output streams, using temp objects because
        // member streams are final
        try {
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) { }

        mmInStream = tmpIn;
        mmOutStream = tmpOut;
    }

    @Override
    public void run() {
        byte[] buffer = new byte[1024];  // buffer store for the stream
        int bytes; // bytes returned from read()
        // Keep listening to the InputStream until an exception occurs
        while (true) {
            try {
                // Read from the InputStream
                bytes = mmInStream.available();

                if(bytes != 0) {
                    buffer = new byte[1024];
                    SystemClock.sleep(100); //pause and wait for rest of data. Adjust this depending on your sending speed.
                    bytes = mmInStream.available(); // how many bytes are ready to be read?
                    bytes = mmInStream.read(buffer, 0, bytes); // record how many bytes we actually read
                    mHandler.obtainMessage(MainActivity.MESSAGE_READ, bytes, -1, buffer)
                            .sendToTarget(); // Send the obtained bytes to the UI activity

                }
            } catch (IOException e) {
                e.printStackTrace();
                isConnected = false; // Connection lost
                sendBluetoothNotification();
                break;
            }
        }


    }

    // Method to check if the thread is connected
    public boolean isConnected() {
        return isConnected;
    }

    /* Call this from the main activity to send data to the remote device */
    public void write(String input) {
        byte[] bytes = input.getBytes();           //converts entered String into bytes
        try {
            mmOutStream.write(bytes);
        } catch (IOException e) { }
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }

    private void sendBluetoothNotification() {
        // Create a notification channel (required for Android Oreo and above)
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(NOTIFICATION_CHANNEL_ID,
                    "Bluetooth Notification Channel",
                    NotificationManager.IMPORTANCE_DEFAULT);
            channel.setDescription("Notification for Bluetooth disconnection");
            channel.enableLights(true);
            channel.setLightColor(Color.BLUE);
            channel.enableVibration(true);
            channel.setVibrationPattern(new long[]{0, 1000, 500, 1000});

            NotificationManager notificationManager = (NotificationManager) mContext.getSystemService(Context.NOTIFICATION_SERVICE);
            notificationManager.createNotificationChannel(channel);
        }

        // Create a notification
        Notification.Builder builder = new Notification.Builder(mContext, NOTIFICATION_CHANNEL_ID)
                .setContentTitle("Bluetooth Disconnected")
                .setContentText("The Bluetooth connection was lost.");
                //.setSmallIcon(R.drawable.ic_notification);


        // Notify the user
        NotificationManager notificationManager = (NotificationManager) mContext.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(NOTIFICATION_ID, builder.build());
    }
}