# 维护 tolangc

## 贡献代码

1. 提出 Issue（可选）
2. 在 Issue 下提出认领该 Issue（可选）
3. 管理员将 Issue 分配给认领者
4. 签出分支进行开发
5. 创建 Pull Request，并关联到该 Issue
6. 管理员对 Pull Request 进行 Review
7. 管理员同意 Pull Request，将分支合并到主分支

## 修改教程

教程位于 `docs/tutorials/` 下。教程文件名格式为 `<no>-<titile>.md`，例如 `01-compiler-tutorial-title.md`。章节顺序见 [README.md](README.md) 的 `Todo` 部分。如需在教程中插入图片，请将图片放在 `docs/tutorials/imgs/chapter<no>/` 文件夹下。

## 单元测试

单元测试位于 `tests/` 下。尽量为每一个模块编写单元测试，但不对覆盖率进行要求。单元测试文件名格式为 `test_<module>.cpp`，例如 `test_module_name.cpp`。

本项目使用 [doctest](https://github.com/doctest/doctest) 实现单元测试。doctest 的使用方法见其文档。
