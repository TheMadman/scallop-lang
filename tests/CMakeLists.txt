function(testcase target)
	add_executable(test_${target} ${target}.c)
	target_link_libraries(test_${target} scalloplang)
	add_test(NAME ${target} COMMAND test_${target})
endfunction()

testcase(scallop_lang_lex)
testcase(scallop_lang_token)
