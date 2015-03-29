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
		
		String config = "{\"turnHost\":\"203.195.185.236\",\"turnPort\":3488,\"compCount\":2}";
		
		final EIce caller = EIce.newCaller(config);
		final String callerContent = caller.getLocalContent();
		Log.i(TAG, "callerContent=" + callerContent);
		
		final EIce callee = EIce.newCallee(config, callerContent);
		final String calleeContent = callee.getLocalContent();
		Log.i(TAG, "calleeContent=" + calleeContent);
		
		
		new Thread(new Runnable(){

			@Override
			public void run() {
				caller.callerNego(calleeContent, new EIceListener() {
					@Override
					public void onNegoResult(String negoResult) {
						Log.i(TAG, "caller nego result = " + negoResult);
					}
				});
			}
			
		}).start();
		
		callee.calleeNego(new EIceListener() {
			@Override
			public void onNegoResult(String negoResult) {
				Log.i(TAG, "callee nego result = " + negoResult);
			}
		});
		
		try {
			caller.waitforNegoResult();
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
		
		try {
			callee.waitforNegoResult();
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
		
		caller.freeCall();
		callee.freeCall();
		
	}
}
