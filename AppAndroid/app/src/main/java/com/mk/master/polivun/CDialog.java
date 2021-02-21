package com.mk.master.polivun;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.widget.Toast;

import com.google.android.material.snackbar.Snackbar;

/**
 * Класс для отображения диалогов
 */
public class CDialog {

    /**
     * Показать Snackbar
     * @param in_Activity Активити
     * @param in_Message Текст сообщения
     */
    public static void Snackbar(Activity in_Activity, String in_Message){
        Snackbar.make(in_Activity.findViewById(android.R.id.content), in_Message, Snackbar.LENGTH_SHORT);
    }

    /**
     * Показать Toast сообщение
     * @param in_Activity Активити
     * @param in_Message Текст сообщения
     */
    public static void Toast(Context in_Activity, String in_Message){
        Toast.makeText(in_Activity.getApplicationContext(), in_Message, Toast.LENGTH_SHORT).show();
    }

    /**
     * Показать ProgressDialog
     * @param in_Activity Активити
     * @param in_Message Текст сообщения
     */
    public static ProgressDialog ProcessDialog(Context in_Activity, String in_Message){
        ProgressDialog mDialog = new ProgressDialog(in_Activity);
        mDialog.setMessage(in_Message);
        mDialog.setCancelable(false);
        mDialog.show();
        return mDialog;
    }
}
