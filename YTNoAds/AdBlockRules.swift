//
//  AdBlockRules.swift
//  YTNoAds
//
//  Created by HASZ2K on 12/11/2026.
//

import Foundation

enum AdBlockRules {
    private static let blockedURLPatterns = [
        #"doubleclick\.net"#,
        #"googlesyndication\.com"#,
        #"googleadservices\.com"#,
        #"youtube\.com/pagead/"#,
        #"youtube\.com/api/stats/ads"#,
        #"youtube\.com/ptracking"#,
        #"youtube\.com/get_midroll"#,
        #"youtube\.com/youtubei/v1/player/ad_break"#,
    ]

    static func encodedJSON() throws -> String {
        let rules = blockedURLPatterns.map { pattern in
            [
                "trigger": ["url-filter": pattern],
                "action": ["type": "block"],
            ]
        }
        let data = try JSONSerialization.data(withJSONObject: rules)
        return String(decoding: data, as: UTF8.self)
    }
}
