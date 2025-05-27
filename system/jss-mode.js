ace.define("ace/mode/jss", ["require", "exports", "module", "ace/lib/oop", "ace/mode/text", "ace/mode/text_highlight_rules", "ace/mode/javascript_highlight_rules", "ace/mode/html_highlight_rules"], function(require, exports, module) {
    var oop = require("../lib/oop");
    var TextMode = require("./text").Mode;
    var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;
    var JavaScriptHighlightRules = require("./javascript_highlight_rules").JavaScriptHighlightRules;
    var HtmlHighlightRules = require("./html_highlight_rules").HtmlHighlightRules;

    var JssHighlightRules = function() {
        // JavaScriptのハイライトルールを継承
        var jsRules = new JavaScriptHighlightRules().getRules();
        var htmlRules = new HtmlHighlightRules().getRules();

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
                // JavaScriptのキーワード
                {
                    token: "keyword",
                    regex: "\\b(break|case|catch|continue|debugger|default|delete|do|else|finally|for|function|if|in|instanceof|new|null|return|switch|this|throw|try|typeof|var|void|while|with|class|const|export|extends|import|let|static|super|yield|async|await|of|get|set)\\b"
                },
                // HTMLのキーワード
                {
                    token: "keyword",
                    regex: "\\b(html|head|body|div|span|p|a|img|script|style|link|meta|title|form|input|button|select|option|table|tr|td|th|ul|ol|li|h1|h2|h3|h4|h5|h6)\\b"
                },
                // 文字列
                {
                    token: "string",
                    regex: '["\']',
                    next: "string"
                },
                // 数値
                {
                    token: "constant.numeric",
                    regex: "\\b\\d+\\b"
                },
                // コメント
                {
                    token: "comment",
                    regex: "//.*$"
                },
                {
                    token: "comment",
                    regex: "/\\*",
                    next: "comment"
                },
                // その他のテキスト
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
            ],
            "comment": [
                {
                    token: "comment",
                    regex: "\\*/",
                    next: "php"
                },
                {
                    token: "comment",
                    regex: "."
                }
            ]
        };

        // JavaScriptとHTMLのルールをマージ
        this.embedRules(jsRules, "js-", [{
            token: "comment.block",
            regex: "\\?>",
            next: "start"
        }]);
        this.embedRules(htmlRules, "html-", [{
            token: "comment.block",
            regex: "\\?>",
            next: "start"
        }]);
    };

    oop.inherits(JssHighlightRules, TextHighlightRules);
    exports.JssHighlightRules = JssHighlightRules;

    var Mode = function() {
        this.HighlightRules = JssHighlightRules;
    };
    oop.inherits(Mode, TextMode);
    exports.Mode = Mode;
});