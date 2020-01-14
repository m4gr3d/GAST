package com.google.vr.youtube.gast.app

import android.graphics.SurfaceTexture
import android.media.MediaPlayer
import android.util.Log
import android.view.Surface
import com.google.vr.youtube.gast.GastActivity
import org.godotengine.godot.gast.GastManager

class MainActivity : GastActivity() {

    private val mediaPlayer: MediaPlayer by lazy {
        MediaPlayer.create(applicationContext, R.raw.ytvr_ar)
    }

    override fun onGLGodotMainLoopStarted() {
        super.onGLGodotMainLoopStarted()

        val texId = GastManager.getTextureIdForNode(GastManager.DEFAULT_GROUP_NAME)
        Log.d("FHK", "Retrieved $texId from GastManager.")

        runOnUiThread {
            val surfaceTexture = SurfaceTexture(texId)
            surfaceTexture.setDefaultBufferSize(640, 480)
            val surface = Surface(surfaceTexture)

            mediaPlayer.setSurface(surface)
            mediaPlayer.isLooping = true
//            mediaPlayer.start()
        }
    }

    override fun onStop() {
        super.onStop()

        mediaPlayer.release()
        finish()
    }
}
