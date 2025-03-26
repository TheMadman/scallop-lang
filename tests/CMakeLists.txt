function(testcase target)
	add_executable(test_${target} ${target}.c)
	target_link_libraries(test_${target} scallop-lang)
	add_test(NAME ${target} COMMAND test_${target})
endfunction()

testcase(scallop_lang_classifier)
testcase(scallop_lang_lex)
