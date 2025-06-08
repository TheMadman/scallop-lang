{
	stdenv,
	cmake,
	libadt,
}:

stdenv.mkDerivation {
	pname = "scallop-lang";
	version = "0.0.1";
	src = ./.;

	nativeBuildInputs = [cmake];
	buildInputs = [
		libadt
	];

	cmakeFlags = [
		"-DBUILD_TESTING=True"
	];

	doCheck = true;
	checkTarget = "test";
}
