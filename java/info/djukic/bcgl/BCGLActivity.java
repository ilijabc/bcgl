package info.djukic.bcgl;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.inputmethod.InputMethodManager;

public class BCGLActivity extends Activity {

    private static final String TAG = "BCGLActivity";

    private static BCGLActivity mActivity;
    private BCGLView mView;

    public static BCGLActivity getInstance() {
        return mActivity;
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        Log.d(TAG, "onConfigurationChanged: newConfig=" + newConfig);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        BCGLLib.nativeInitFileSystem(
            getAssets(),
            getFilesDir().getAbsolutePath(),
            android.os.Environment.getExternalStorageDirectory().getAbsolutePath());

        mView = new BCGLView(this);
        setContentView(mView);

        mActivity = this;
        BCGLLib.nativeAppChangeState(BCGLLib.EVENT_APP_CREATE);
        Log.d(TAG, "onCreate");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mActivity = null;
        BCGLLib.nativeAppChangeState(BCGLLib.EVENT_APP_DESTROY);
        Log.d(TAG, "onDestroy");
    }

    @Override
    protected void onStart() {
        super.onStart();
        BCGLLib.nativeAppChangeState(BCGLLib.EVENT_APP_START);
        Log.d(TAG, "onStart");
    }

    @Override
    protected void onStop() {
        super.onStop();
        BCGLLib.nativeAppChangeState(BCGLLib.EVENT_APP_STOP);
        Log.d(TAG, "onStop");
    }

    @Override
    protected void onPause() {
        super.onPause();
        BCGLLib.nativeAppChangeState(BCGLLib.EVENT_APP_PAUSE);
        Log.d(TAG, "onPause");
    }

    @Override
    protected void onResume() {
        super.onResume();
        BCGLLib.nativeAppChangeState(BCGLLib.EVENT_APP_RESUME);
        Log.d(TAG, "onResume");
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int pointerIndex = event.getActionIndex();
        int pointerId = event.getPointerId(pointerIndex);
        int maskedAction = event.getActionMasked();
        int numPointers = event.getPointerCount();
        switch (maskedAction) {
        case MotionEvent.ACTION_DOWN:
        case MotionEvent.ACTION_POINTER_DOWN:
            BCGLLib.nativeTouchEvent(BCGLLib.EVENT_TOUCH_DOWN, pointerId, event.getX(pointerIndex), event.getY(pointerIndex));
            break;
        case MotionEvent.ACTION_MOVE:
            for (int i = 0; i < numPointers; i++) {
                BCGLLib.nativeTouchEvent(BCGLLib.EVENT_TOUCH_MOVE, event.getPointerId(i), event.getX(i), event.getY(i));
            }
            break;
        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
        case MotionEvent.ACTION_CANCEL:
            BCGLLib.nativeTouchEvent(BCGLLib.EVENT_TOUCH_UP, pointerId, event.getX(pointerIndex), event.getY(pointerIndex));
            break;
        }
        return true;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        BCGLLib.nativeKeyEvent(BCGLLib.EVENT_KEY_DOWN, keyCode, event.getScanCode());
        return true;
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        BCGLLib.nativeKeyEvent(BCGLLib.EVENT_KEY_UP, keyCode, event.getScanCode());
        return true;
    }

    @Override
    public void onBackPressed() {
    }

    private int convertOrientation(int i) {
        switch (i) {
            case 0:
                return ActivityInfo.SCREEN_ORIENTATION_USER;
            case 1:
                return ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
            case 2:
                return ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
        }
        return ActivityInfo.SCREEN_ORIENTATION_SENSOR;
    }

    public void showKeyboard(boolean show) {
        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        mView.requestFocus();
        imm.showSoftInput(mView, 0);
    }

    public void changeOrientation(int orientation) {
        setRequestedOrientation(convertOrientation(orientation));
    }
}
