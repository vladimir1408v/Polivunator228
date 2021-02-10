package com.mk.master.polivun;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.telephony.SmsManager;
import android.view.View;

public class MainActivity extends AppCompatActivity {

    String m_sTell = "+79630332479";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void PolivOn(View view) throws Exception {
        sendSmsMessage("ON");
    }

    public void PolivOff(View view) throws Exception {
        sendSmsMessage("OFF");
    }


    private void sendSmsMessage(String message) throws Exception
    {
        SmsManager smsMgr = SmsManager.getDefault();
        smsMgr.sendTextMessage(m_sTell, null, message, null, null);
    }
}