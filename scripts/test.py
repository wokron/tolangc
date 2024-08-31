import argparse
import os
import pathlib
import subprocess
from typing import Literal

TEST_CACHE_DIR = pathlib.Path(".test_cache")
TEST_CACHE_DIR.mkdir(exist_ok=True)

MARS_PATH = os.environ.get("MARS_PATH", "mars.jar")


class TestStat:
    def __init__(self, passed: int = 0, failed: int = 0):
        self.passed = passed
        self.failed = failed

    @property
    def is_success(self):
        return self.failed == 0

    @property
    def total(self):
        return self.passed + self.failed

    def __add__(self, other: "TestStat"):
        return TestStat(self.passed + other.passed, self.failed + other.failed)

    def __str__(self) -> str:
        return f"total/pass/fail: {self.total}/{self.passed}/{self.failed}"


def get_tolangc(backend: Literal["llvm", "pcode"]):
    return TEST_CACHE_DIR / "build" / backend / "bin" / "tolangc"


def build(backend: Literal["llvm", "pcode"]):
    print(f"Building target with backend: {backend}...", end=" ", flush=True)
    subprocess.run(
        ["bash", "./scripts/build.sh", backend],
        stdout=subprocess.DEVNULL,
        check=True,
        env=dict(os.environ) | {"BUILD_DIR": TEST_CACHE_DIR / "build" / backend},
    )
    print("done!")


def get_output_file(test_file: pathlib.Path):
    return test_file.with_suffix(".output")


def get_input_file(test_file: pathlib.Path):
    return test_file.with_suffix(".input")


def general_test(test_files: list[pathlib.Path], preprocess_fn, run_fn, compare_fn):
    passed = 0
    failed = 0
    for test_file in test_files:
        input_file = get_input_file(test_file)
        output_file = get_output_file(test_file)

        if not output_file.exists():
            raise FileNotFoundError(f"Output file not found: {output_file}")

        preprocess_result = preprocess_fn(test_file)
        run_result = run_fn(
            preprocess_result, input_file if input_file.exists() else None
        )
        is_success = compare_fn(run_result, output_file)
        if is_success:
            print(f"✓ passed {test_file}")
            passed += 1
        else:
            failed += 1
            print(f"x failed {test_file}")

    return TestStat(passed, failed)


def compare(test_result_file: pathlib.Path, output_file: pathlib.Path):

    def is_not_empty_line(text: str):
        return len(text.strip()) > 0

    def convert_to_float(text: str):
        try:
            val = float(text)
        except ValueError:
            val = f"'{text}'"
        return val

    with open(test_result_file, "r") as f:
        test_results = map(convert_to_float, filter(is_not_empty_line, f.readlines()))
    with open(output_file, "r") as f:
        expected_results = map(
            convert_to_float, filter(is_not_empty_line, f.readlines())
        )

    is_success = True
    try:
        for no, (test_result, expected_result) in enumerate(
            zip(test_results, expected_results, strict=True)
        ):
            if type(test_result) != float or abs(test_result - expected_result) > 1e-6:
                is_success = False
                print(f"Error: line {no + 1}: {test_result} != {expected_result}")
    except ValueError:
        is_success = False
        print("Error: different number of lines")

    return is_success


def test_llvm(test_files: list[pathlib.Path]):
    def preprocess(test_file: pathlib.Path):
        target_file = TEST_CACHE_DIR / f"{test_file.stem}.ll"
        # compile the test file
        subprocess.run(
            [get_tolangc("llvm"), test_file, "--emit-ir", "-o", target_file],
            check=True,
        )
        if not target_file.exists():
            raise FileNotFoundError(f"Compilation result not found: {target_file}")
        return target_file

    def run(target_file: pathlib.Path, input_file: pathlib.Path):
        # run the compiled file using llvm_run.sh
        test_result_file = TEST_CACHE_DIR / f"{target_file.stem}.output"
        with open(test_result_file, "w") as f:
            subprocess.run(
                ["bash", "./scripts/llvm_run.sh", target_file],
                check=True,
                stdin=open(input_file, "r") if input_file else None,
                stdout=f,
            )
        if not test_result_file.exists():
            raise FileNotFoundError(f"Test result not found: {test_result_file}")
        return test_result_file

    return general_test(test_files, preprocess, run, compare)


def test_pcode(test_files: list[pathlib.Path]):
    def preprocess(test_file: pathlib.Path):
        return test_file

    def run(test_file: pathlib.Path, input_file: pathlib.Path):
        test_result_file = TEST_CACHE_DIR / f"{test_file.stem}.output"
        with open(test_result_file, "w") as f:
            subprocess.run(
                [get_tolangc("pcode"), test_file],
                check=True,
                stdin=open(input_file, "r") if input_file else None,
                stdout=f,
            )
        if not test_result_file.exists():
            raise FileNotFoundError(f"Test result not found: {test_result_file}")
        return test_result_file

    return general_test(test_files, preprocess, run, compare)


def test_mips(test_files: list[pathlib.Path]):
    def preprocess(test_file: pathlib.Path):
        target_file = TEST_CACHE_DIR / f"{test_file.stem}.s"
        # compile the test file
        subprocess.run(
            [get_tolangc("llvm"), test_file, "-S", "-o", target_file],
            check=True,
        )
        if not target_file.exists():
            raise FileNotFoundError(f"Compilation result not found: {target_file}")
        return target_file

    def run(target_file: pathlib.Path, input_file: pathlib.Path):
        # run the compiled file using mips_run.sh
        test_result_file = TEST_CACHE_DIR / f"{target_file.stem}.output"
        with open(test_result_file, "w") as f:
            subprocess.run(
                ["bash", "./scripts/mips_run.sh", target_file],
                check=True,
                stdin=open(input_file, "r") if input_file else None,
                stdout=f,
                env={"MARS_PATH": MARS_PATH},
            )
        if not test_result_file.exists():
            raise FileNotFoundError(f"Test result not found: {test_result_file}")
        return test_result_file

    return general_test(test_files, preprocess, run, compare)


def format_separator(ch: str, size: int, description: str | None = None):
    assert len(ch) == 1
    assert size >= 0
    if description:
        rest_size = max(size - len(description) - 2, 0)
        left_size = rest_size // 2
        right_size = rest_size - left_size
        return f"{ch * left_size} {description} {ch * right_size}"
    else:
        return ch * size


def test(args):
    test_stages: list[Literal["llvm", "pcode", "mips"]] = args.stage
    test_files = args.file if args.file else []

    if args.dir:
        # get all test files in the directory
        for test_dir in args.dir:
            test_dir = pathlib.Path(test_dir)
            test_files += list(test_dir.glob("*.tol"))

    test_files = list(map(pathlib.Path, test_files))

    total_stat = TestStat()

    for stage in test_stages:
        if stage == "llvm":
            print(format_separator("=", 42, "llvm"))
            build("llvm")
            stat = test_llvm(test_files)
            print(f"LLVM test result: {stat}")
            total_stat += stat
        elif stage == "mips":
            print(format_separator("=", 42, "mips"))
            build("llvm")
            stat = test_mips(test_files)
            print(f"MIPS test result: {stat}")
            total_stat += stat
        elif stage == "pcode":
            print(format_separator("=", 42, "pcode"))
            build("pcode")
            stat = test_pcode(test_files)
            print(f"Pcode test result: {stat}")
            total_stat += stat
        else:
            raise ValueError(f"Invalid stage: {stage}")

    print(format_separator("=", 42))
    print(f"Total test result: {total_stat}")

    if not total_stat.is_success:
        exit(1)


def get_args():
    parser = argparse.ArgumentParser(description="Integration test script for tolangc")

    parser.add_argument(
        "-s",
        "--stage",
        type=str,
        action="append",
        required=True,
        choices=["llvm", "mips", "pcode"],
        help="stage to test",
    )
    parser.add_argument(
        "-d",
        "--dir",
        type=str,
        action="append",
        help="directory of test cases",
    )
    parser.add_argument(
        "-f", "--file", type=str, action="append", help="test case to run"
    )

    args = parser.parse_args()
    return args


if __name__ == "__main__":
    args = get_args()
    test(args)
