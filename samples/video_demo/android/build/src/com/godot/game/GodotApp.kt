/** */
/*  GodotApp.java                                                        */
/** */
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/** */
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/** */

package com.godot.game

import android.graphics.SurfaceTexture
import android.media.MediaPlayer
import android.util.Log
import android.view.Surface
import com.google.vr.youtube.gast.GastManager
import org.godotengine.godot.Godot
import org.godotengine.godot.plugin.GodotPluginRegistry
import java.lang.IllegalStateException
import java.util.concurrent.atomic.AtomicInteger

/**
 * Template activity for Godot Android custom builds.
 * Feel free to extend and modify this class for your custom logic.
 */
class GodotApp : Godot(), GastManager.GLCallbackListener {

    companion object {
        const val TAG = "FHK"
    }

    private val updateTextureImageCounter = AtomicInteger()

    private val mediaPlayer: MediaPlayer by lazy {
        MediaPlayer.create(applicationContext, R.raw.flight)
    }

    private var surfaceTexture: SurfaceTexture? = null

    override fun onGLGodotMainLoopStarted() {
        super.onGLGodotMainLoopStarted()

        Log.d(TAG, "Retrieving Gast plugin.")
        val gastPlugin = GodotPluginRegistry.getPluginRegistry().getPlugin("Gast")
        if (gastPlugin !is GastManager) {
            throw IllegalStateException("Invalid plugin type.")
        }

        Log.d(TAG, "Creating mesh instance")
        val nodePath = gastPlugin.createMeshInstance("/root/Spatial")

        Log.d(TAG, "Registering GL callback listener")
        gastPlugin.addGLCallbackListener(nodePath, this)

        Log.d(TAG, "Setting up mesh instance with path $nodePath")
        gastPlugin.setupMeshInstance(nodePath)

        Log.d(TAG, "Retrieving texture id")
        val texId =
            gastPlugin.getExternalTextureId(nodePath)
        Log.d("FHK", "Retrieved $texId from GastManager.")

        runOnUiThread {
            surfaceTexture = SurfaceTexture(texId)
            surfaceTexture?.setDefaultBufferSize(640, 480)
            surfaceTexture?.setOnFrameAvailableListener {
                updateTextureImageCounter.incrementAndGet()
            }
            val surface = Surface(surfaceTexture)

            mediaPlayer.setSurface(surface)
            mediaPlayer.isLooping = true
            mediaPlayer.start()
        }
    }

    override fun onStop() {
        super.onStop()

        mediaPlayer.release()
        finish()
    }

    override fun onGLProcess(nodePath: String, delta: Float) {
        var counter = updateTextureImageCounter.get()
        while (counter > 0) {
            surfaceTexture?.updateTexImage()
            counter = updateTextureImageCounter.decrementAndGet()
        }
    }
}
