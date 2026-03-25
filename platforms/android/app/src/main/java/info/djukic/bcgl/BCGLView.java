package info.djukic.bcgl;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class BCGLView extends SurfaceView implements SurfaceHolder.Callback {

    private static final String TAG = "BCGLView";

    public BCGLView(Context context) {
        super(context);
        init(context);
    }

    public BCGLView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public BCGLView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    public BCGLView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(context);
    }

    private void init(Context context) {
        getHolder().setFormat(PixelFormat.RGBA_8888);
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        BCGLLib.nativeSurfaceCreated(0, surfaceHolder.getSurface());
        Log.d(TAG, "surfaceCreated");
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
        BCGLLib.nativeSurfaceChanged(0, surfaceHolder.getSurface(), format, width, height);
        Log.d(TAG, "surfaceChanged: " + width + ", " + height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        BCGLLib.nativeSurfaceDestroyed(0, surfaceHolder.getSurface());
        Log.d(TAG, "surfaceDestroyed");
    }
}
