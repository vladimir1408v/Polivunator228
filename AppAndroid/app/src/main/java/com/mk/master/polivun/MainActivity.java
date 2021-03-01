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
import android.view.MotionEvent;
import android.view.View;
import android.widget.TabHost;
import android.widget.TimePicker;

import com.google.android.material.tabs.TabLayout;

import java.util.Calendar;

public class MainActivity extends AppCompatActivity {

    CProperty m_Property;
    CDialog m_Dialog;
    String m_sSendData = "";

    TabHost tabHost;

    float lastX = 0;

    Calendar dateAndTime=Calendar.getInstance();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        m_Property =  new CProperty(this);

        setTitle("Полив");
        tabHost = (TabHost) findViewById(R.id.tabHost);
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

    // установка обработчика выбора времени
    TimePickerDialog.OnTimeSetListener t=new TimePickerDialog.OnTimeSetListener() {
        public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
            dateAndTime.set(Calendar.HOUR_OF_DAY, hourOfDay);
            dateAndTime.set(Calendar.MINUTE, minute);
            try {
                setInitialDateTime();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    };

    // установка начальных даты и времени
    private void setInitialDateTime() throws Exception {
        sendSmsMessage(m_sSendData + " " + DateUtils.formatDateTime(this, dateAndTime.getTimeInMillis(), DateUtils.FORMAT_SHOW_TIME));
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
                m_sSendData = "0";
                setTime();
                break;
            case R.id.poliv2:
                m_sSendData = "1";
                setTime();
            case R.id.poliv3:
                m_sSendData = "2";
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

    @Override
    public boolean onTouchEvent(MotionEvent touchevent) {
        switch (touchevent.getAction()) {
            // when user first touches the screen to swap
            case MotionEvent.ACTION_DOWN: {
                lastX = touchevent.getX();
                break;
            }
            case MotionEvent.ACTION_UP: {
                float currentX = touchevent.getX();

                // if left to right swipe on screen
                if (lastX < currentX) {

                    switchTabs(false);
                }

                // if right to left swipe on screen
                if (lastX > currentX) {
                    switchTabs(true);
                }

                break;
            }
        }
        return false;
    }

    public void switchTabs(boolean direction) {
        if (direction) // true = move left
        {
            if (tabHost.getCurrentTab() == 0)
                tabHost.setCurrentTab(tabHost.getTabWidget().getTabCount() - 1);
            else
                tabHost.setCurrentTab(tabHost.getCurrentTab() - 1);
        } else
        // move right
        {
            if (tabHost.getCurrentTab() != (tabHost.getTabWidget()
                    .getTabCount() - 1))
                tabHost.setCurrentTab(tabHost.getCurrentTab() + 1);
            else
                tabHost.setCurrentTab(0);
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