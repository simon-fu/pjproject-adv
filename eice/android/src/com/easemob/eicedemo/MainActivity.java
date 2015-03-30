package com.easemob.eicedemo;

import com.easemob.media.EIce;
import com.easemob.media.EIce.EIceListener;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

public class MainActivity extends ActionBarActivity {

	protected final String TAG = "EIceDemo";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		runDemo();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	protected void runDemo(){
		
		final String config = "{\"turnHost\":\"203.195.185.236\",\"turnPort\":3488,\"compCount\":2}";
		
		final String[] callerContents = new String[]{null};
		final String[] calleeContents = new String[]{null};
		
		Thread thread = new Thread(new Runnable(){

			@Override
			public void run() {
				final EIce caller = EIce.newCaller(config);
				final String callerContent = caller.getLocalContent();
				Log.i(TAG, "callerContent=" + callerContent);
				synchronized (callerContents) {
					callerContents[0] = callerContent;
					callerContents.notifyAll();
				}
				
				// wait for callee content
				synchronized (calleeContents) {
					if(calleeContents[0] == null){
						try {
							calleeContents.wait();
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
				}
				
				caller.callerNego(calleeContents[0], new EIceListener() {
					@Override
					public void onNegoResult(String negoResult) {
						Log.i(TAG, "caller nego result = " + negoResult);
					}
				});
				
				try {
					caller.waitforNegoResult();
				} catch (InterruptedException e1) {
					e1.printStackTrace();
				}
				
				caller.freeCall();
			}
			
		});
		thread.start();
		
		// wait for caller content
		synchronized (callerContents) {
			if(callerContents[0] == null){
				try {
					callerContents.wait();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
		
		final EIce callee = EIce.newCallee(config, callerContents[0]);
		final String calleeContent = callee.getLocalContent();
		Log.i(TAG, "calleeContent=" + calleeContent);
		synchronized (calleeContents) {
			calleeContents[0] = calleeContent;
			calleeContents.notifyAll();
		}
		
		callee.calleeNego(new EIceListener() {
			@Override
			public void onNegoResult(String negoResult) {
				Log.i(TAG, "callee nego result = " + negoResult);
			}
		});
		
		
		
		try {
			callee.waitforNegoResult();
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
		callee.freeCall();
		
		
		
		try {
			thread.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
}
