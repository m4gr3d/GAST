package org.godotengine.plugin.gast.samples.xrapp.xrmediaviewer

import android.app.Application
import com.google.android.exoplayer2.Player
import com.google.android.exoplayer2.SimpleExoPlayer
import com.google.android.exoplayer2.source.ConcatenatingMediaSource
import com.google.android.exoplayer2.source.LoopingMediaSource
import com.google.android.exoplayer2.source.ProgressiveMediaSource
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory
import com.google.android.exoplayer2.upstream.RawResourceDataSource
import com.google.android.exoplayer2.util.Util

/**
 * Application component for the XR Media Viewer sample app.
 *
 * Sets up the exoplayer instance so it's available to all app's components.
 */
class XrMediaViewerApplication : Application() {

    internal var videoPlayer: SimpleExoPlayer? = null
        private set

    override fun onCreate() {
        super.onCreate()

        videoPlayer = SimpleExoPlayer.Builder(applicationContext).build().apply {
            // Setup player playlist
            val dataSourceFactory = DefaultDataSourceFactory(
                applicationContext,
                Util.getUserAgent(applicationContext, "XR Media Viewer")
            )
            val videosSource = ConcatenatingMediaSource()

            videosSource.addMediaSource(
                ProgressiveMediaSource.Factory(dataSourceFactory)
                    .setTag(R.raw.big_buck_bunny_480p)
                    .createMediaSource(RawResourceDataSource.buildRawResourceUri(R.raw.big_buck_bunny_480p))
            )
            videosSource.addMediaSource(
                ProgressiveMediaSource.Factory(dataSourceFactory)
                    .setTag(R.raw.flight)
                    .createMediaSource(RawResourceDataSource.buildRawResourceUri(R.raw.flight))
            )

            prepare(LoopingMediaSource(videosSource))
        }
    }
}
