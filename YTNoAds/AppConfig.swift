//
//  AppConfig.swift
//  YTNoAds
//
//  Created by HASZ2K on 12/11/2026.
//

import Foundation

enum AppConfig {
    static let homeURL = URL(string: "https://www.youtube.com")!
    static let ruleListIdentifier = "pl.ytnoads.blocklist"
    static let logSubsystem = "pl.ytnoads"
    static let minimumWindowSize = CGSize(width: 900, height: 600)

    static let userAgent = [
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)",
        "AppleWebKit/605.1.15 (KHTML, like Gecko)",
        "Version/17.4 Safari/605.1.15",
    ].joined(separator: " ")

    private static let internalHostSuffixes: Set<String> = [
        "youtube.com",
        "google.com",
        "gstatic.com",
        "ytimg.com",
        "googlevideo.com",
    ]

    static func isInternal(_ url: URL) -> Bool {
        guard let host = url.host?.lowercased() else { return false }
        return internalHostSuffixes.contains { host == $0 || host.hasSuffix(".\($0)") }
    }
}
