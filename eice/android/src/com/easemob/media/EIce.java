package com.easemob.media;

import android.util.Log;

public class EIce {
	protected static final String TAG = "EIce_Java";
	protected native static void nativeInitEIce();
	protected native long nativeNewCaller(String config);
	protected native long nativeNewCallee(String config, String remoteContent);
	protected native String nativeGetLocalContent(long handle);
	protected native void nativeCallerNego(long handle, String remoteContent);
	protected native void nativeFreeCall(long handle);
	protected native String nativeGetNegoResult(long handle);
	protected long nativeHandle = 0;
	protected String localContent = null;
	protected String negoResult = null;
	protected Thread queryThread = null;
	protected boolean stopReq = false;
	
//	protected static EIce sInst = new EIce();
	static {
		System.loadLibrary("eice");
//		System.loadLibrary("eice_other");
//		System.loadLibrary("easemob_jni");
		nativeInitEIce();
	}
	
	private EIce(){
		
	}
	
	public static EIce newCaller(String config){
		try{
			Log.i(TAG, "newCaller: ==>; ");
			EIce o = new EIce();
			o.nativeHandle = o.nativeNewCaller(config);
			o.localContent = o.nativeGetLocalContent(o.nativeHandle);
			return o;
		}finally{
			Log.i(TAG, "newCaller: <==; ");
		}
		
	}
	
	public static EIce newCallee(String config, String remoteContent){
		try{
			Log.i(TAG, "newCallee: ==>; ");
			EIce o = new EIce();
			o.nativeHandle = o.nativeNewCallee(config, remoteContent);
			o.localContent = o.nativeGetLocalContent(o.nativeHandle);
			return o;
		}finally{
			Log.i(TAG, "newCallee: <==; ");
		}
		
		
	}
	
	public String getLocalContent(){
		return this.localContent;
	}
	
	protected void startQueryResult(final EIceListener listener){
		this.queryThread = new Thread(new Runnable(){

			@Override
			public void run() {
				Log.i(TAG, "queryThread start, " + this.hashCode());
				while(true){
					
					String result = nativeGetNegoResult(nativeHandle);
					
					
					if(result != null){
						Log.i(TAG, "got nego result: " + result);
						
						synchronized (EIce.this) {
							EIce.this.negoResult = result;
							EIce.this.notifyAll();
						}
						
						if(listener != null){
							listener.onNegoResult(result);
						}
						break;
					}
					try {
						Thread.sleep(200);
					} catch (InterruptedException e) {
						Log.i(TAG, "queryThread got InterruptedException " + e.getLocalizedMessage());
						break;
					}
					
					if(stopReq){
						Log.i(TAG, "queryThread got stop req");
						break;
					}
				}
				Log.i(TAG, "queryThread exit; " + this.hashCode());
			}
			
		});
		this.stopReq = false;
		this.queryThread.start();
	}
	
	
	private boolean negoing = false;
	public void callerNego(String remoteContent, final EIceListener listener){
		Log.i(TAG, "callerNego: ==>; " + this.hashCode());
		synchronized (this) {
			if(nativeHandle == 0) throw new IllegalStateException("callerNego handle null");
			if(negoing){
				Log.e(TAG, "callerNego: already negoing; " + this.hashCode());
				return ;
			}
			
			negoing = true;
			this.nativeCallerNego(this.nativeHandle, remoteContent);
			startQueryResult(listener);
			
		}
		Log.i(TAG, "callerNego: <==; " + this.hashCode());
	}
	
	
	public void calleeNego(EIceListener listener){
		Log.i(TAG, "calleeNego: ==>; " + this.hashCode());
		synchronized (this) {
			if(nativeHandle == 0) throw new IllegalStateException("calleeNego handle null");
			if(negoing){
				Log.e(TAG, "calleeNego: already negoing; " + this.hashCode());
				return ;
			}
			negoing = true;
			startQueryResult(listener);
		}
		Log.i(TAG, "calleeNego: <==; " + this.hashCode());
	}
	
	public void waitforNegoResult() throws InterruptedException{
		Log.i(TAG, "waitforNegoResult: ==>; " + this.hashCode());
		while(true){
			synchronized (this) {
				if(nativeHandle == 0) throw new IllegalStateException("calleeNego handle null");
				
				if(this.getNegoResult() == null){
					this.wait();
				}else{
					break;
				}
			}
		}
		Log.i(TAG, "waitforNegoResult: <==; " + this.hashCode());
	}
	
	public String getNegoResult(){
		Log.i(TAG, "getNegoResult: ==>; " + this.hashCode());
		try{
			synchronized (EIce.this) {
				return EIce.this.negoResult;
			}
		}finally{
			Log.i(TAG, "getNegoResult: <==; " + this.hashCode());
		}
		
	}
	
	public void freeCall(){
		Log.i(TAG, "freeCall: ==>; " + this.hashCode());
		if(nativeHandle == 0) throw new IllegalStateException("calleeNego handle null");
		
		if(this.queryThread != null){
			Log.i(TAG, "freeCall: stop query thread... ; " + this.hashCode());
			this.stopReq = true;
			try {
				this.queryThread.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			this.queryThread = null;
			Log.i(TAG, "freeCall: stop query thread done ; " + this.hashCode());
		}
		this.nativeFreeCall(this.nativeHandle);
		this.nativeHandle = 0;
		Log.i(TAG, "freeCall: <==; " + this.hashCode());
	}
	
	public static interface EIceListener{
		public void onNegoResult(String negoResult);
	}
}
