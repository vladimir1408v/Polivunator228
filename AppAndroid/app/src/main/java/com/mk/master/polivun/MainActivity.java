package com.mk.master.polivun;

import androidx.appcompat.app.AppCompatActivity;
import androidx.viewpager.widget.ViewPager;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TabHost;

import com.google.android.material.tabs.TabLayout;

public class MainActivity extends AppCompatActivity {

    CProperty m_Property;
    CDialog m_Dialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        m_Property =  new CProperty(this);

        setTitle("Поливун");
        TabHost tabHost = (TabHost) findViewById(R.id.tabHost);
        tabHost.setup();
        TabHost.TabSpec tabSpec = tabHost.newTabSpec("tag1");
        tabSpec.setContent(R.id.tab1);
        tabSpec.setIndicator("Полив");
        tabHost.addTab(tabSpec);
        tabSpec = tabHost.newTabSpec("tag2");
        tabSpec.setContent(R.id.tab2);
        tabSpec.setIndicator("Время");
        tabHost.addTab(tabSpec);

        tabHost.setCurrentTab(0);

    }

    @SuppressLint("NonConstantResourceId")
    public void onClick(View v) throws Exception {
        switch (v.getId())
        {
            case R.id.button:
                sendSmsMessage("ON");
                break;
            case R.id.button4:
                sendSmsMessage("OFF");
                break;
        }
    }

    @SuppressLint("ResourceType")
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch(id){
            case R.id.action_settings :
                startActivity(new Intent(this, Preferences.class));
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void sendSmsMessage(String message) throws Exception
    {
        try {
            SmsManager smsMgr = SmsManager.getDefault();
            smsMgr.sendTextMessage(m_Property.Get("Tell", ""), null, message, null, null);
        }catch (Exception e){
            m_Dialog.Toast(this, e.getMessage());
        }
    }
}