package hm10.insper.com.hm10test;

import android.app.Activity;
import android.app.ListActivity;
import android.bluetooth.*;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Parcelable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.UUID;

public class ServiceConnectActivity extends ListActivity {

    public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";
    private static final String TAG = "ServiceConnectActivity" ;

    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothDevice mBluetoothDevice;
    private LeServiceListAdapter leServiceListAdapter;
    private BluetoothGatt mBluetoothGatt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        leServiceListAdapter = new LeServiceListAdapter();
        setListAdapter(leServiceListAdapter);
        final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        Bundle bundle = getIntent().getExtras();
        mBluetoothDevice = mBluetoothAdapter.getRemoteDevice(bundle.getString(EXTRAS_DEVICE_ADDRESS));
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_screenconnect);
    }

    @Override
    protected void onResume() {
        mBluetoothGatt = mBluetoothDevice.connectGatt(this, false, mBluetoothGattCallback);
        super.onResume();
    }

    @Override
    protected void onPause() {
        mBluetoothGatt.disconnect();
        leServiceListAdapter.clear();
        super.onPause();
    }

    private final BluetoothGattCallback mBluetoothGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.i(TAG, "Connected to GATT server.");
                mBluetoothGatt.discoverServices();
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.i(TAG, "Disconnected to GATT server.");
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            for(final BluetoothGattService service: gatt.getServices()) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        leServiceListAdapter.addUuid(service.getUuid());
                        leServiceListAdapter.notifyDataSetChanged();}
                });
            }

            super.onServicesDiscovered(gatt, status);
        }
    };

    private class LeServiceListAdapter extends BaseAdapter {
        private ArrayList<UUID> uuidArrayList;
        private LayoutInflater mInflator;

        public LeServiceListAdapter() {
            uuidArrayList = new ArrayList<>();
            mInflator = ServiceConnectActivity.this.getLayoutInflater();
        }

        public void clear() {
            uuidArrayList.clear();
        }

        public void addUuid(UUID uuid) {
            uuidArrayList.add(uuid);
        }
        @Override
        public int getCount() {
            return uuidArrayList.size();
        }

        @Override
        public Object getItem(int position) {
            return uuidArrayList.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View view, ViewGroup parent) {
            ViewHolder viewHolder;
            // General ListView optimization code.
            if (view == null) {
                view = mInflator.inflate(R.layout.activity_screenconnect, null);
                viewHolder = new ViewHolder();
                viewHolder.serviceUuid = (TextView) view.findViewById(R.id.service_name);
                view.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) view.getTag();
            }
            UUID uuid = uuidArrayList.get(position);
            if (uuid != null && uuid.toString().length() > 0) {
                viewHolder.serviceUuid.setText(uuid.toString());
            }
            return view;
        }
    }

    static class ViewHolder {
        TextView serviceUuid;

    }
}
