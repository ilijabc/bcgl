package info.djukic.bcgl;

import android.content.res.AssetManager;
import android.view.Surface;
import android.view.View;

public class BCGLLib {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public static final int EVENT_APP_CREATE = 1;
    public static final int EVENT_APP_DESTROY = 2;
    public static final int EVENT_APP_START = 3;
    public static final int EVENT_APP_STOP = 4;
    public static final int EVENT_APP_RESUME = 5;
    public static final int EVENT_APP_PAUSE = 6;

    public static final int EVENT_TOUCH_DOWN = 1;
    public static final int EVENT_TOUCH_MOVE = 2;
    public static final int EVENT_TOUCH_UP = 3;

    public static final int EVENT_KEY_DOWN = 1;
    public static final int EVENT_KEY_UP = 2;

    public static final int EVENT_TEXT_INPUT = 1;
    public static final int EVENT_TEXT_CANCEL = 2;

    public static final int MSG_FINISH_ACTIVITY = 1;
    public static final int MSG_SHOW_KEYBOARD = 2;
    public static final int MSG_SET_ORIENTATION = 3;
    public static final int MSG_INPUT_TEXT_DIALOG = 4;
    public static final int MSG_SET_WINDOW_TYPE = 5;

    public static final int GET_NUMBER_DENSITY = 1;

    public static final int GET_INTEGER_KEYBOARD = 1;

    public static native void nativeInitFileSystem(AssetManager manager, String local_path, String external_path);
    public static native void nativeSurfaceCreated(int id, Surface surface);
    public static native void nativeSurfaceChanged(int id, Surface surface, int format, int width, int height);
    public static native void nativeSurfaceDestroyed(int id, Surface surface);
    public static native void nativeAppChangeState(int state);
    public static native void nativeTouchEvent(int event, int id, float x, float y);
    public static native boolean nativeKeyEvent(int event, int key, int code, int deviceId);
    public static native void nativeTextEvent(int event, String text);

    public static void onNativeMessage(int type, int x, int y, String text) {
        if (BCGLActivity.getInstance() == null)
            return;
        switch (type) {
            case MSG_FINISH_ACTIVITY:
                BCGLActivity.getInstance().finish();
                break;
            case MSG_SHOW_KEYBOARD:
                BCGLActivity.getInstance().showKeyboard(x == 1);
                break;
            case MSG_SET_ORIENTATION:
                BCGLActivity.getInstance().changeOrientation(x);
                break;
            case MSG_INPUT_TEXT_DIALOG:
                BCGLActivity.getInstance().inputTextDialog(text);
                break;
            case MSG_SET_WINDOW_TYPE:
                BCGLActivity.getInstance().setWindowType(x);
                break;
        }
    }

    public static float onNativeGetNumber(int key) {
        if (BCGLActivity.getInstance() == null)
            return 0.0f;
        switch (key) {
            case GET_NUMBER_DENSITY:
                return BCGLActivity.getInstance().getResources().getDisplayMetrics().density;
        }
        return 0.0f;
    }

    public static int onNativeGetInteger(int key) {
        if (BCGLActivity.getInstance() == null)
            return 0;
        switch (key) {
            case GET_INTEGER_KEYBOARD:
                return BCGLActivity.getInstance().getResources().getConfiguration().keyboard;
        }
        return 0;
    }
}
