#!/usr/bin/perl
# This is a script to change POS tags in Japanese to English 
# abbreviations

use strict;
use utf8;
use Getopt::Long;
use List::Util qw(sum min max shuffle);
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

my %mapping = (
    "名詞" => "N", # Noun
    "代名詞" => "PRP", # Pronoun
    "連体詞" => "DT", # Adjectival determiner
    "動詞" => "V", # Verb
    "形容詞" => "ADJ", # Adjective
    "形状詞" => "ADJV", # Adjectival verb
    "副詞" => "ADV", # Adverb
    "助詞" => "PRT", # Particle
    "助動詞" => "AUXV", # Auxiliary verb
    "補助記号" => ".", # Punctuation
    "記号" => "SYM", # Symbol
    "接尾辞" => "SUF", # Suffix
    "接頭辞" => "PRE", # Prefix
    "語尾" => "TAIL", # Word tail (conjugation)
    "接続詞" => "CC", # Conjunction
    "代名詞" => "PRP", # Pronoun
    "URL" => "URL", # URL
    "英単語" => "ENG", # English word
    "言いよどみ" => "FIL", # Filler
    "web誤脱" => "MSP", # Misspelling
    "感動詞" => "INT", # Interjection
    "新規未知語" => "UNK", # Unclassified unknown word
    # "" => "",
);

while(<STDIN>) {
    chomp;
    my @arr = map {
        my @col = split(/\//);
        die "Bad POS tag in @col" if not exists $mapping{$col[1]};
        $col[1] = $mapping{$col[1]};
        join("/", @col)
    } split(/ /);
    print "@arr\n";
}