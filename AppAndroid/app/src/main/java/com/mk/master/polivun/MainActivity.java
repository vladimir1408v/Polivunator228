package com.mk.master.polivun;

import androidx.appcompat.app.AppCompatActivity;
import androidx.viewpager.widget.ViewPager;

import android.annotation.SuppressLint;
import android.app.TimePickerDialog;
import android.content.Intent;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.text.format.DateUtils;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TabHost;
import android.widget.TimePicker;

import com.google.android.material.tabs.TabLayout;

import java.util.Calendar;

public class MainActivity extends AppCompatActivity {

    CProperty m_Property;
    CDialog m_Dialog;

    Calendar dateAndTime=Calendar.getInstance();

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

        setInitialDateTime();
    }

    // установка обработчика выбора времени
    TimePickerDialog.OnTimeSetListener t=new TimePickerDialog.OnTimeSetListener() {
        public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
            dateAndTime.set(Calendar.HOUR_OF_DAY, hourOfDay);
            dateAndTime.set(Calendar.MINUTE, minute);
            setInitialDateTime();
        }
    };

    // установка начальных даты и времени
    private void setInitialDateTime() {
        CDialog.Toast(this, DateUtils.formatDateTime(this,
                dateAndTime.getTimeInMillis(),
                DateUtils.FORMAT_SHOW_DATE | DateUtils.FORMAT_SHOW_YEAR
                        | DateUtils.FORMAT_SHOW_TIME));
    }

    // отображаем диалоговое окно для выбора времени
    public void setTime() {
        new TimePickerDialog(MainActivity.this, t,
                dateAndTime.get(Calendar.HOUR_OF_DAY),
                dateAndTime.get(Calendar.MINUTE), true)
                .show();
    }

    @SuppressLint("NonConstantResourceId")
    public void SetTimePoliv(View v) throws Exception {
        switch (v.getId())
        {
            case R.id.poliv1:
                setTime();
                break;
            case R.id.poliv2:
                setTime();
                break;
        }
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