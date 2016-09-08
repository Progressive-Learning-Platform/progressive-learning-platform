define("ace/mode/assembly_plp_highlight_rules",["require","exports","module","ace/lib/oop","ace/mode/text_highlight_rules"],  function(require, exports, module)
{
  "use strict";

  var oop = require("../lib/oop");
  var TextHighlightRules = require("./text_highlight_rules").TextHighlightRules;

  var AssemblyPLPHighlightRules = function()
  {
    // regexp must not have capturing parenthesis. Use (?:) instead.
    // regexps are ordered-> the match is used
    this.$rules =
    {
      start:
      [
        {
          token: 'entity.name.function.label.colon',
          regex: '\\b(?:[a-zA-Z](?:[a-zA-Z]|_|[0-9])*(?:[a-zA-Z]|[0-9])+):\\B'
        },
        {
          token: 'keyword.instruction.normal',
          regex: '\\b(?:addu|subu|and|or|nor|slt|sltu|sll|srl|mullo|mulhi|jr|jalr|j|jal|addiu|andi|ori|slti|sltiu|lui|lw|sw|beq|bne)\\b'
        },
        {
          token: 'keyword.instruction.pseudo',
          regex: '\\b(?:nop|b|move|li|push|pop)\\b'
        },
        {
          token: 'entity.name.function.label.plain',
          regex: '\\b(?:[a-zA-Z](?:[a-zA-Z]|_|[0-9])*(?:[a-zA-Z]|[0-9])+)\\b'
        },
        {
          token: 'constant.other.address.parenthesis',
          regex: '\\(\\$(?:[a-zA-Z]|[0-9])+\\)'
        },
        {
          token: 'constant.other.address.plain',
          regex: '\\$(?:[a-zA-Z]|[0-9])+'
        },
        {
          token: 'comment.line',
          regex: '#.*$'
        },
        {
          token: 'constant.character.comma',
          regex: ','
        },
        {
          token: 'keyword.directive',
          regex: '\\B\\.(?:.+?)\\b'
        },
        {
          token: 'string.quoted',
          regex: '\"(?:.*?)\"'
        },
        {
          token: 'constant.numeric',
          regex: '\\b(?:(?:\\d)+|0x(?:[0-9a-fA-F]+))\\b'
        }

      ]
    }
    this.normalizeRules();
  };

  AssemblyPLPHighlightRules.metaData = { fileTypes: [ 'asm' ],
        name: 'Assembly PLP', }


  oop.inherits(AssemblyPLPHighlightRules, TextHighlightRules);

  exports.AssemblyPLPHighlightRules = AssemblyPLPHighlightRules;

});

define("ace/mode/plp",["require","exports","module","ace/lib/oop","ace/mode/text","ace/mode/assembly_plp_highlight_rules","ace/mode/folding/coffee"], function(require, exports, module) {
"use strict";

var oop = require("../lib/oop");
var TextMode = require("./text").Mode;
var AssemblyPLPHighlightRules = require("./assembly_plp_highlight_rules").AssemblyPLPHighlightRules;

var Mode = function() {
    this.HighlightRules = AssemblyPLPHighlightRules;
};
oop.inherits(Mode, TextMode);

(function() {
    this.lineCommentStart = ";";
    this.$id = "ace/mode/plp";
}).call(Mode.prototype);

exports.Mode = Mode;
});
