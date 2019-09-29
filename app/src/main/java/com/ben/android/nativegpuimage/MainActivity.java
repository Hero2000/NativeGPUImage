package com.ben.android.nativegpuimage;

import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.opengl.GLUtils;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;

import com.ben.android.library.GPUImageRender;
import com.ben.android.library.NGPNativeBridge;
import com.ben.android.library.filter.DilationFilter;
import com.ben.android.library.filter.GPUImage3x3ConvolutionFilter;
import com.ben.android.library.filter.GPUImageAddBlendFilter;
import com.ben.android.library.filter.GPUImageAlphaBlendFilter;
import com.ben.android.library.filter.GPUImageDissolveBlendFilter;
import com.ben.android.library.filter.GPUImageDivideBlendFilter;
import com.ben.android.library.filter.GPUImageEmbossFilter;
import com.ben.android.library.filter.GPUImageExclusionBlendFilter;
import com.ben.android.library.filter.GPUImageExposureFilter;
import com.ben.android.library.filter.GPUImageFalseColorFilter;
import com.ben.android.library.filter.GPUImageGammaFilter;
import com.ben.android.library.filter.GPUImageGrayscaleFilter;
import com.ben.android.library.filter.GPUImageSobelEdgeDetectionFilter;
import com.ben.android.library.filter.GPUImageToonFilter;
import com.ben.android.library.filter.GPUImageVibranceFilter;
import com.ben.android.library.filter.GPUImageVignetteFilter;
import com.ben.android.library.filter.GPUImageWeakPixelInclusionFilter;
import com.ben.android.library.filter.GPUImageWhiteBalanceFilter;
import com.ben.android.library.filter.GaussianBlurFilter;
import com.ben.android.library.filter.PixelationFilter;
import com.ben.android.library.filter.ZoomBlurFilter;

public class MainActivity extends AppCompatActivity {
    private GPUImageRender gpuImageRender;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        gpuImageRender = findViewById(R.id.mGPUImageRender);
        NGPNativeBridge.init();

    }

    public void setFilter(View view) {
//        NGPNativeBridge.setFilter(new PixelationFilter(30f));
//        NGPNativeBridge.setFilter(new ZoomBlurFilter(new PointF(0.7f,0.7f),1.0f));
//        NGPNativeBridge.setFilter(new DilationFilter(1));
//        NGPNativeBridge.setFilter(new GaussianBlurFilter(100f));
//        NGPNativeBridge.setFilter(new GPUImageWhiteBalanceFilter(1000f,1.0f));
//        NGPNativeBridge.setFilter(new GPUImageWeakPixelInclusionFilter());
//        NGPNativeBridge.setFilter(new GPUImageVignetteFilter(new PointF(0.5f,0.5f), new float[]{0.0f, 0.0f, 0.0f}, 0.2f, 0.75f));
//        NGPNativeBridge.setFilter(new GPUImageVibranceFilter(1.5f));
//        NGPNativeBridge.setFilter(new GPUImageAddBlendFilter(BitmapFactory.decodeResource(getResources(), R.drawable.lookup_amatorka)));
//        NGPNativeBridge.setFilter(new GPUImageAddBlendFilter(createWatermark("ben622")));
//        NGPNativeBridge.setFilter(new GPUImageToonFilter(0.5f,20.0f));
//        NGPNativeBridge.setFilter(new GPUImageGammaFilter(2.0f));
//        NGPNativeBridge.setFilter(new GPUImageFalseColorFilter());
//        NGPNativeBridge.setFilter(new GPUImageExposureFilter());
//        NGPNativeBridge.setFilter(new GPUImageExclusionBlendFilter());
//        NGPNativeBridge.setFilter(new GPUImage3x3ConvolutionFilter(new float[]{0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}));
//        NGPNativeBridge.setFilter(new GPUImageEmbossFilter());
//        NGPNativeBridge.setFilter(new GPUImageDivideBlendFilter(createWatermark("ben622")));
//        NGPNativeBridge.setFilter(new GPUImageAlphaBlendFilter(BitmapFactory.decodeResource(getResources(), R.drawable.lookup_amatorka),0.5f));
//        NGPNativeBridge.setFilter(new GPUImageDissolveBlendFilter(BitmapFactory.decodeResource(getResources(), R.drawable.lookup_amatorka),1.0f));
//        NGPNativeBridge.setFilter(new GPUImageSobelEdgeDetectionFilter(1.7f));
        NGPNativeBridge.setFilter(new GPUImageGrayscaleFilter());

        reqeustRender();
    }

    public void renderBitmap(View view) {
        Bitmap bitmap = BitmapFactory.decodeResource(getResources(), com.ben.android.library.R.mipmap.dog);
        Bitmap resizedBitmap = Bitmap.createBitmap(bitmap.getWidth(), bitmap.getHeight(),
                Bitmap.Config.ARGB_8888);
        Canvas can = new Canvas(resizedBitmap);
        can.drawARGB(0x00, 0x00, 0x00, 0x00);
        can.drawBitmap(bitmap, 0, 0, null);

        NGPNativeBridge.setBitmap(resizedBitmap);
        reqeustRender();
    }

    public void reqeustRender() {
        NGPNativeBridge.requestRender();
    }

    public void capture(View view) {
        /*int width = gpuImageRender.getMeasuredWidth();
        int height = gpuImageRender.getMeasuredHeight();
        Bitmap resultBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        NGPNativeBridge.capture(resultBitmap);
        //show
        ImageView imageView = new ImageView(this);
        imageView.setImageBitmap(resultBitmap);
        new AlertDialog.Builder(this)
                .setView(imageView)
                .create().show();*/

        NGPNativeBridge.nativeCreateGL();

    }

    public Bitmap createWatermark(String value) {
        Bitmap bitmap = Bitmap.createBitmap(100, 100, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setColor(Color.WHITE);
        paint.setTextSize(20);
        canvas.drawText(value, 30, 30, paint);
        return bitmap;
    }
}
