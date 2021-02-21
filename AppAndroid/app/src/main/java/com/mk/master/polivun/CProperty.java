package com.mk.master.polivun;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

/**
 * Класс для работы с настройками прилодения
 * Позволяет хранить и получать данные из Preferences
 */
public class CProperty {
    SharedPreferences m_Settings;
    Activity m_Activity;
    String m_FileName;

    /**
     * Конструктор
     * @param in_Activity Активити
     * @param in_FileName Имя файла настроек
     */
    public CProperty(Activity in_Activity, String in_FileName){
        m_Activity = in_Activity;
        m_FileName = in_FileName;
        m_Settings = m_Activity.getSharedPreferences(m_FileName, Context.MODE_PRIVATE);
    }

    public CProperty(Activity in_Activity){
        m_Activity = in_Activity;
        m_Settings = PreferenceManager.getDefaultSharedPreferences(in_Activity);
    }

    /**
     * Получить строку по ключу из Preferences
     * @param in_PrefName Ключ параметра
     * @param in_Default Дефолтное значение
     * @return Строка из Preferences
     */
    public String Get(String in_PrefName, String in_Default){
        return m_Settings.getString(in_PrefName,in_Default);
    }

    /**
     * Получить boolean по ключу из Preferences
     * @param in_PrefName Ключ параметра
     * @param in_Default Дефолтное значение
     * @return Boolean из Preferences
     */
    public boolean Get(String in_PrefName, boolean in_Default){
        return m_Settings.getBoolean(in_PrefName, in_Default);
    }

    /**
     * Установить новое значение по имени для Preferences
     * @param in_PrefName Ключ параметра
     * @param in_Value Дефолтное значение
     */
    public void Set(String in_PrefName, String in_Value){
        SharedPreferences.Editor prefEditor = m_Settings.edit();
        prefEditor.putString(in_PrefName, in_Value);
        prefEditor.apply();
    }

    /**
     * Установить новое значение по имени для Preferences
     * @param in_PrefName Ключ параметра
     * @param in_Value Дефолтное значение
     */
    public void Set(String in_PrefName, boolean in_Value){
        SharedPreferences.Editor prefEditor = m_Settings.edit();
        prefEditor.putBoolean(in_PrefName, in_Value);
        prefEditor.apply();
    }
}
