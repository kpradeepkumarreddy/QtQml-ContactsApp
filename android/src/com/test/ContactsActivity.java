package com.test;

import org.qtproject.qt.android.bindings.QtActivity;
import android.util.Log;
import android.os.Bundle;
import java.util.ArrayList;
import android.provider.*;
import android.content.*;
import android.database.Cursor;
import java.util.HashSet;
import android.os.Build;
import android.Manifest;
import android.content.pm.PackageManager;
import android.widget.Toast;
import android.database.ContentObserver;
import android.os.Handler;
import android.net.Uri;
import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONException;
import android.os.RemoteException;
import java.util.Collection;


 public class ContactsActivity extends QtActivity {

    native void onContactsReceived(String contacts);

    private static final int PERMISSIONS_REQUEST_READ_CONTACTS = 100;
    private static final int PERMISSIONS_REQUEST_WRITE_CONTACTS = 101;

    private ContactsObserver contactsObserver = this.new ContactsObserver(new Handler());

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        getContentResolver().unregisterContentObserver(contactsObserver);
    }

    private void getContacts() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M &&
        checkSelfPermission(Manifest.permission.READ_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
             requestPermissions(new String[]{Manifest.permission.READ_CONTACTS}, PERMISSIONS_REQUEST_READ_CONTACTS);
         }else{
            getContentResolver().registerContentObserver(ContactsContract.CommonDataKinds.Phone.CONTENT_URI, true, contactsObserver);
            onContactsReceived(fetchContacts());
         }
    }

    @Override
   public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
       if (requestCode == PERMISSIONS_REQUEST_READ_CONTACTS) {
           if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                getContentResolver().registerContentObserver(ContactsContract.CommonDataKinds.Phone.CONTENT_URI, true, contactsObserver);
                onContactsReceived(fetchContacts());
           } else {
               Toast.makeText(this, "Until you grant the permission, we cannot display the names", Toast.LENGTH_SHORT).show();
           }
       }else if(requestCode == PERMISSIONS_REQUEST_WRITE_CONTACTS){
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                addDummyContacts();
            } else {
                Toast.makeText(this, "Until you grant the permission, we cannot write to contacts", Toast.LENGTH_SHORT).show();
            }
       }
   }

    private String fetchContacts(){
        JSONArray jsonArrContacts = new JSONArray();

        String[] PROJECTION = new String[]{
                ContactsContract.CommonDataKinds.Phone.CONTACT_ID,
                ContactsContract.Contacts.DISPLAY_NAME,
                ContactsContract.CommonDataKinds.Phone.NUMBER
        };
        Cursor cursor = getContentResolver().query(ContactsContract.CommonDataKinds.Phone.CONTENT_URI, PROJECTION, null, null,
        ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME + " ASC");
        if (cursor != null) {
            HashSet<String> mobileNoSet = new HashSet<String>();
            try {
                final int nameIndex = cursor.getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME);
                final int numberIndex = cursor.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER);

                String name, number;
                while (cursor.moveToNext()) {
                    name = cursor.getString(nameIndex);
                    number = cursor.getString(numberIndex);
                    number = number.replace(" ", "");
                    if (!mobileNoSet.contains(number)) {
                        JSONObject jsonContactObj = new JSONObject();
                        jsonContactObj.put("name", name);
                        jsonContactObj.put("phoneNumber", number);
                        jsonArrContacts.put(jsonContactObj);
                        mobileNoSet.add(number);
                    }
                }
            }catch(JSONException jsonEx){

            } finally {
                cursor.close();
            }
        }
        return jsonArrContacts.toString();
    }

    class ContactsObserver extends ContentObserver{
        ContactsObserver(Handler handler){
            super(handler);
        }

        @Override
        public void onChange(boolean selfChange, Uri uri){
            onContactsReceived(fetchContacts());
        }
    }

    void writeContacts(){
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M &&
            checkSelfPermission(Manifest.permission.WRITE_CONTACTS) != PackageManager.PERMISSION_GRANTED) {
             requestPermissions(new String[]{Manifest.permission.WRITE_CONTACTS}, PERMISSIONS_REQUEST_WRITE_CONTACTS);
        }else{
            addDummyContacts();
         }
    }

    void addDummyContacts(){
        for(int i=0; i<1000; i++){
            ArrayList<ContentProviderOperation> contentProviderOperations
                        = new ArrayList<ContentProviderOperation>();

                contentProviderOperations.add(ContentProviderOperation.newInsert(
                        ContactsContract.RawContacts.CONTENT_URI)
                        .withValue(ContactsContract.RawContacts.ACCOUNT_TYPE, null)
                        .withValue(ContactsContract.RawContacts.ACCOUNT_NAME, null)
                        .build());

                // Adding Name
                contentProviderOperations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID, 0)
                        .withValue(ContactsContract.Data.MIMETYPE,
                                ContactsContract.CommonDataKinds.StructuredName.CONTENT_ITEM_TYPE)
                        .withValue(ContactsContract.CommonDataKinds.StructuredName.DISPLAY_NAME, "test-"+i)
                        .build());

                // Adding Number
                contentProviderOperations.add(ContentProviderOperation
                        .newInsert(ContactsContract.Data.CONTENT_URI)
                        .withValueBackReference(ContactsContract.Data.RAW_CONTACT_ID, 0)
                        .withValue(ContactsContract.Data.MIMETYPE,
                                ContactsContract.CommonDataKinds.Phone.CONTENT_ITEM_TYPE)
                        .withValue(ContactsContract.CommonDataKinds.Phone.NUMBER, "0"+i)
                        .withValue(ContactsContract.CommonDataKinds.Phone.TYPE,
                                ContactsContract.CommonDataKinds.Phone.TYPE_WORK)
                        .build());
                try {
                    getContentResolver().applyBatch(ContactsContract.AUTHORITY, contentProviderOperations);
                } catch (OperationApplicationException e) {
                    e.printStackTrace();
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
    }
}
