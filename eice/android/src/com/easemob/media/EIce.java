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
		nativeInitEIce();
	}
	
	private EIce(){
		
	}
	
	public static EIce newCaller(String config){
		EIce o = new EIce();
		o.nativeHandle = o.nativeNewCaller(config);
		o.localContent = o.nativeGetLocalContent(o.nativeHandle);
		return o;
	}
	
	public static EIce newCallee(String config, String remoteContent){
		EIce o = new EIce();
		o.nativeHandle = o.nativeNewCallee(config, remoteContent);
		o.localContent = o.nativeGetLocalContent(o.nativeHandle);
		return o;
	}
	
	public String getLocalContent(){
		return this.localContent;
	}
	
	protected void startQueryResult(final EIceListener listener){
		this.queryThread = new Thread(new Runnable(){

			@Override
			public void run() {
				while(true){
					
					String result = nativeGetNegoResult(nativeHandle);
					
					
					if(result != null){
						synchronized (EIce.this) {
							EIce.this.negoResult = result;
							EIce.this.notifyAll();
						}
						
						Log.i(TAG, "got nego result: " + result);
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
				Log.i(TAG, "queryThread exit");
			}
			
		});
		this.stopReq = false;
		this.queryThread.start();
	}
	
	public void callerNego(String remoteContent, final EIceListener listener){
		this.nativeCallerNego(this.nativeHandle, remoteContent);
		startQueryResult(listener);
	}
	
	public void calleeNego(EIceListener listener){
		startQueryResult(listener);
	}
	
	public void waitforNegoResult() throws InterruptedException{
		while(true){
			synchronized (this) {
				if(this.getNegoResult() == null){
					this.wait();
				}else{
					break;
				}
			}
		}
		
	}
	
	public String getNegoResult(){
		synchronized (EIce.this) {
			return EIce.this.negoResult;
		}
	}
	
	public void freeCall(){
		if(this.queryThread != null){
			this.stopReq = true;
			try {
				this.queryThread.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			this.queryThread = null;
			
		}
		this.nativeFreeCall(this.nativeHandle);
		this.nativeHandle = 0;
	}
	
	public static interface EIceListener{
		public void onNegoResult(String negoResult);
	}
}
