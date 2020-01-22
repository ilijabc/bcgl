package info.djukic.bcgl;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.TextView;

public class BCGLActivity extends Activity {

    private static final String TAG = "BCGLActivity";

    private static BCGLActivity mActivity;
    private BCGLView mGLView;
    private AlertDialog mDialog;
    private EditText mEditText;

    private int mWindowType;

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
            getFilesDir().getAbsolutePath() + "/",
            android.os.Environment.getExternalStorageDirectory().getAbsolutePath() + "/");

        mGLView = new BCGLView(this);
        setContentView(mGLView);

        mEditText = new EditText(this);
        mEditText.setImeOptions(EditorInfo.IME_ACTION_DONE);
        mEditText.setInputType(InputType.TYPE_CLASS_TEXT);
        mEditText.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView textView, int actionId, KeyEvent event) {
                if ((event != null && (event.getKeyCode() == KeyEvent.KEYCODE_ENTER)) || (actionId == EditorInfo.IME_ACTION_DONE)) {
                    BCGLLib.nativeTextEvent(BCGLLib.EVENT_TEXT_INPUT, mEditText.getText().toString());
                    mDialog.dismiss();
                    return true;
                }
                return false;
            }
        });

        mDialog = new AlertDialog.Builder(this)
                .setMessage("Enter text")
                .setView(mEditText)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        BCGLLib.nativeTextEvent(BCGLLib.EVENT_TEXT_INPUT, mEditText.getText().toString());
                    }
                })
                .setNegativeButton("Cancel", null)
                .setOnDismissListener(new DialogInterface.OnDismissListener() {
                    @Override
                    public void onDismiss(DialogInterface dialogInterface) {
                        BCGLLib.nativeTextEvent(BCGLLib.EVENT_TEXT_CANCEL, null);
                        setWindowType(mWindowType);
                    }
                })
                .create();
        mDialog.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);

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

    public void showKeyboard(final boolean show) {
        Log.d(TAG, "SHOW KEYBOARD: " + show);
    }

    public void changeOrientation(int orientation) {
        setRequestedOrientation(convertOrientation(orientation));
    }

    public void inputTextDialog(final String text) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mEditText.setText(text);
                mEditText.setSelection(text.length());
                mDialog.show();
            }
        });
    }

    public void setWindowType(final int type) {
        mWindowType = type;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (type == 1) { // BC_DISPLAY_FULLSCREEN
                    getWindow().getDecorView().setSystemUiVisibility(
                            View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                                    View.SYSTEM_UI_FLAG_FULLSCREEN |
                                    View.SYSTEM_UI_FLAG_LOW_PROFILE |
                                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    );
                } else {
                    getWindow().getDecorView().setSystemUiVisibility(
                            View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    );
                }
            }
        });
    }
}
