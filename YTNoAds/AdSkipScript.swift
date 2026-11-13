//
//  AdSkipScript.swift
//  YTNoAds
//
//  Created by HASZ2K on 12/11/2026.
//

import Foundation

enum AdSkipScript {
    private static let hiddenSelectors = [
        ".ytp-ad-overlay-container",
        ".ytp-ad-text-overlay",
        ".ytp-ad-image-overlay",
        "ytd-ad-slot-renderer",
        "ytd-in-feed-ad-layout-renderer",
        "ytd-display-ad-renderer",
        "ytd-promoted-sparkles-web-renderer",
        "ytd-banner-promo-renderer",
        "ytd-companion-slot-renderer",
        "ytd-player-legacy-desktop-watch-ads-renderer",
        "#masthead-ad",
        "#player-ads",
        "#panels ytd-ads-engagement-panel-content-renderer",
    ]

    private static let skipButtonSelectors = [
        ".ytp-ad-skip-button",
        ".ytp-ad-skip-button-modern",
        ".ytp-skip-ad-button",
    ]

    private static let pollIntervalMs = 250

    static var source: String {
        """
        (() => {
          'use strict';
          const HIDDEN_SELECTORS = \(encodeJSON(hiddenSelectors));
          const SKIP_SELECTORS = \(encodeJSON(skipButtonSelectors));
          const POLL_INTERVAL_MS = \(pollIntervalMs);

          const style = document.createElement('style');
          style.textContent = `${HIDDEN_SELECTORS.join(',')} { display: none !important; }`;
          (document.head || document.documentElement).appendChild(style);

          let mutedByScript = false;

          const fastForwardAd = (video) => {
            if (!video.muted) {
              video.muted = true;
              mutedByScript = true;
            }
            video.playbackRate = 16;
            if (Number.isFinite(video.duration) && video.duration > 0) {
              video.currentTime = video.duration;
            }
            document.querySelectorAll(SKIP_SELECTORS.join(',')).forEach((button) => button.click());
          };

          const restorePlayback = (video) => {
            if (!mutedByScript) return;
            video.muted = false;
            video.playbackRate = 1;
            mutedByScript = false;
          };

          setInterval(() => {
            const player = document.querySelector('.html5-video-player');
            const video = document.querySelector('video.html5-main-video') || document.querySelector('video');
            if (!player || !video) return;
            if (player.classList.contains('ad-showing')) {
              fastForwardAd(video);
            } else {
              restorePlayback(video);
            }
          }, POLL_INTERVAL_MS);
        })();
        """
    }

    private static func encodeJSON(_ items: [String]) -> String {
        guard let data = try? JSONSerialization.data(withJSONObject: items) else { return "[]" }
        return String(decoding: data, as: UTF8.self)
    }
}
