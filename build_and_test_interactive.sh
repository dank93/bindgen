./build.sh && ./test/bind_test_code.sh && ./test/build_generated_bindings.sh && (cd test/BUILD/; ipython -ic "import generated_pybind as gp")
