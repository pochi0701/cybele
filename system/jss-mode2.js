// カスタムモードの定義
ace.define("ace/mode/jss", ["require", "exports", "module", "ace/lib/oop", "ace/mode/text", "ace/mode/text_highlight_rules"], function(require, exports, module) {
    var oop = require("../lib/oop");
    var TextMode = require("./text").Mode;
    var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;

    var JssHighlightRules = function() {
        this.$rules = {
            "start": [
                {
                    token: "comment.block",
                    regex: "<\\?",
                    next: "php"
                },
                {
                    token: "text",
                    regex: "."
                }
            ],
            "php": [
                {
                    token: "comment.block",
                    regex: "\\?>",
                    next: "start"
                },
                {
                    token: "keyword",
                    regex: "\\b(if|else|while|for|function|var|return)\\b"
                },
                {
                    token: "string",
                    regex: '["\']',
                    next: "string"
                },
                {
                    token: "constant.numeric",
                    regex: "\\b\\d+\\b"
                },
                {
                    token: "text",
                    regex: "."
                }
            ],
            "string": [
                {
                    token: "string",
                    regex: '["\']',
                    next: "php"
                },
                {
                    token: "string",
                    regex: "."
                }
            ]
        };
    };

    oop.inherits(JssHighlightRules, TextHighlightRules);
    exports.JssHighlightRules = JssHighlightRules;

    var Mode = function() {
        this.HighlightRules = JssHighlightRules;
    };
    oop.inherits(Mode, TextMode);
    exports.Mode = Mode;
});