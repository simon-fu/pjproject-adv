package com.easemob.media;

import android.util.Log;

/**
 * this is an internal class which is not supposed to be used by 3rd party.
 * @author easemob
 *
 */
public class EMVideoCallBridge implements IGxStatusCallback{
    
    private static EMVideoCallBridge instance = null;
    
    private EMVideoCallBridge(){
    }
    
    public static EMVideoCallBridge getInstance() {
        if (instance == null) {
            instance = new EMVideoCallBridge();
        }
        return instance;
    }
    
    /**
     * JIN callback API. Please Don't Change it.
     */
    public static boolean createGLContext(int majorVersion, int minorVersion) {
        Log.d("SDL", "to call initEGL");
        return false;
//        return EMVideoCallBridgePrivate.createGLContext(majorVersion, minorVersion);
    }

    /**
     * JIN callback API. Please Don't Change it.
     */
    public static void flipBuffers() {
//    	EMVideoCallBridgePrivate.flipBuffers();
    }

    /**
     * JIN callback API. Please Don't Change it.
     */
    public static void setActivityTitle(String title) {
//    	EMVideoCallBridgePrivate.setActivityTitle(title);
    }
    
    @Override
    public void updateStatus(int status) {
    }
}
