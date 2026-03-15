class ElizaosCpp < Formula
  desc "Next-Generation Cognitive Agent Framework - C++ Implementation"
  homepage "https://github.com/o9nn/elizaos-cpp"
  url "https://github.com/o9nn/elizaos-cpp/archive/refs/tags/v1.0.0.tar.gz"
  sha256 ""
  license "MIT"
  head "https://github.com/o9nn/elizaos-cpp.git", branch: "main"

  depends_on "cmake" => :build
  depends_on "pkg-config" => :build
  depends_on "curl"

  def install
    system "cmake", "-S", ".", "-B", "build",
           "-DCMAKE_BUILD_TYPE=Release",
           *std_cmake_args
    system "cmake", "--build", "build", "--parallel", ENV.make_jobs.to_s
    system "cmake", "--install", "build"
  end

  test do
    system bin/"eliza", "--version" rescue nil
    assert_predicate lib/"libelizaos-core.a", :exist?
  end
end
