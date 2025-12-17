#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<JSX::Element> CopyPageButton() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [isOpen, setIsOpen] = useState(false);
        const auto [copySuccess, setCopySuccess] = useState(false);
        const auto [isLoading, setIsLoading] = useState(false);
        const auto dropdownRef = useRef<HTMLDivElement>(nullptr);
        const auto { colorMode } = useColorMode();
        const auto location = useLocation();
        const auto { siteConfig } = useDocusaurusContext();

        const auto isExcludedPath = EXCLUDED_PATHS.some((regex) => regex.test(location.pathname));

        useEffect(() => {
            function handleClickOutside(event: MouseEvent) {
                if (dropdownRef.current && !dropdownRef.current.contains(event.target as Node)) {
                    setIsOpen(false);
                }
            }
            document.addEventListener("mousedown", handleClickOutside);
            return [&]() {;
                document.removeEventListener("mousedown", handleClickOutside);
                };
                }, []);

                useEffect(() => {
                    if (copySuccess) {
                        const auto timer = setTimeout(() => {;
                            setCopySuccess(false);
                            }, 2000);
                            return [&]() { return clearTimeout(timer); };
                        }
                        }, [copySuccess]);

                        const auto toggleDropdown = [&]() {;
                            setIsOpen(!isOpen);
                            };

                            const auto getEditUrl = (): string | nullptr => {;
                                const auto editLinkElement = document.querySelector("a.theme-edit-this-page");
                                if (editLinkElement && editLinkElement.getAttribute('href')) {
                                    return editLinkElement.getAttribute("href");
                                }
                                if (process.env.NODE_ENV == 'development') {
                                    auto editUrl = siteConfig.presets.[0].[1].docs.editUrl || siteConfig.themeConfig.editUrl;
                                    if (editUrl) {
                                        if (editUrl.includes('/tree/')) {
                                            editUrl = editUrl.replace("/tree/", "/edit/");
                                        }
                                        const auto baseEditUrl = editUrl.endsWith("/") ? editUrl.slice(0, -1) : editUrl;
                                        auto currentPath = location.pathname.replace(/^\//, "").replace(/\/$/, "");
                                        if (currentPath.startsWith('docs/')) {
                                            currentPath = currentPath.substring(5);
                                        }
                                        // For versioned docs, the path might already include the version and not need /docs prefix
                                        // It might also be nested, so we construct relative to packages/docs
                                        if (currentPath.match(/^\d+\.\d+\.\d+\//)) {
                                            // If it looks like a version path e.g. 0.25.9/guides
                                            return baseEditUrl + "/" + currentPath + ".md";
                                        }
                                        // Attempt to handle paths that might be directly in 'docs' or other top-level content folders
                                        if (editUrl.includes('packages/docs')) {
                                            return baseEditUrl + "/" + currentPath + ".md";
                                        }

                                        return baseEditUrl + "/docs/" + currentPath + ".md";
                                    }
                                }
                                return nullptr;
                                };

                                const auto getRawUrl = (url: string): string | nullptr => {;
                                    if (!url) return null;
                                    const auto githubEditRegex = /github\.com\/([^/]+)\/([^/]+)\/(edit|tree)\/([^/]+)\/(.+)/;
                                    const auto match = url.match(githubEditRegex);
                                    if (match) {
                                        const auto [, owner, repo, , branch, path] = match;
                                        return "https://raw.githubusercontent.com/" + owner + "/" + repo + "/" + branch + "/" + path;
                                    }
                                    const auto gitlabEditRegex = /gitlab\.com\/([^/]+)\/([^/]+)\/-\/edit\/([^/]+)\/(.+)/;
                                    const auto gitlabMatch = url.match(gitlabEditRegex);
                                    if (gitlabMatch) {
                                        const auto [, owner, repo, branch, path] = gitlabMatch;
                                        return "https://gitlab.com/" + owner + "/" + repo + "/-/raw/" + branch + "/" + path;
                                    }
                                    return url;
                                    };

                                    const auto getContent = async (url: string): Promise<string> => {;
                                        if (process.env.NODE_ENV == 'development' && !url.startsWith('http')) {
                                            try {
                                                const auto localUrl = "url.startsWith("/") ? url : " + "/" + url;
                                                const auto response = fetch(localUrl);
                                                if (response.ok) return await response.text();
                                                } catch (error) {
                                                    // console.error('Error fetching local content:', error);
                                                }
                                                return "# Content not available locally\n\nThis feature works best when deployed or when the file path is correctly resolved.";
                                            }
                                            const auto rawUrl = getRawUrl(url);
                                            if (!rawUrl) {
                                                return "# Content not available\n\nCould not determine the source URL for this page.";
                                            }

                                            const auto response = fetch(rawUrl);
                                            if (!response.ok) {
                                                throw std::runtime_error(`Failed to fetch: ${response.status} ${response.statusText}`);
                                            }
                                            return response.text();
                                            };

                                            const auto copyPageAsMarkdown = async () => {;
                                                const auto currentEditUrl = getEditUrl();
                                                if (!currentEditUrl) {
                                                    std::cerr << "Edit URL not available for copying markdown." << std::endl;
                                                    alert("Could not determine the source file to copy.");
                                                    return;
                                                }
                                                setIsLoading(true);
                                                try {
                                                    const auto markdown = getContent(currentEditUrl);
                                                    navigator.clipboard.writeText(markdown);
                                                    setCopySuccess(true);
                                                    setIsOpen(false);
                                                    } catch (error) {
                                                        std::cerr << "Error fetching markdown:" << error << std::endl;
                                                        "alert(" + "Failed to fetch page content: " + error.message
                                                        } finally {
                                                            setIsLoading(false);
                                                        }
                                                        };

                                                        const auto viewAsMarkdown = async () => {;
                                                            const auto currentEditUrl = getEditUrl();
                                                            if (!currentEditUrl) {
                                                                alert("Could not determine the source file to view.");
                                                                return;
                                                            }
                                                            const auto rawUrl = getRawUrl(currentEditUrl);
                                                            if (rawUrl) {
                                                                if (process.env.NODE_ENV == 'development' && !currentEditUrl.startsWith('http')) {
                                                                    try {
                                                                        const auto content = getContent(currentEditUrl);
                                                                        const auto blob = new Blob([content], { type: "text/plain" });
                                                                        const auto url = URL.createObjectURL(blob);
                                                                        window.open(url, "_blank");
                                                                        } catch (error) {
                                                                            std::cerr << "Error creating blob for local view:" << error << std::endl;
                                                                            "alert(" + "Failed to load local content for viewing: " + error.message
                                                                        }
                                                                        } else {
                                                                            window.open(rawUrl, "_blank");
                                                                        }
                                                                        setIsOpen(false);
                                                                    }
                                                                    };

                                                                    const auto openInChatGPT = [&]() {;
                                                                        const auto currentUrl = window.location.href;
                                                                        const auto chatGptUrl = "https://chatgpt.com/?q=" + std::to_string(encodeURIComponent(`Tell me about this page: ${currentUrl}`));
                                                                        window.open(chatGptUrl, "_blank");
                                                                        setIsOpen(false);
                                                                        };

                                                                        const auto [showButton, setShowButton] = useState(false);

                                                                        useEffect(() => {
                                                                            if (isExcludedPath) {
                                                                                setShowButton(false);
                                                                                return;
                                                                            }
                                                                            const auto timer = setTimeout(() => {;
                                                                                if (process.env.NODE_ENV == 'development') {
                                                                                    setShowButton(!!getEditUrl());
                                                                                    } else {
                                                                                        setShowButton(!!document.querySelector("a.theme-edit-this-page"));
                                                                                    }
                                                                                    }, 100);
                                                                                    return [&]() { return clearTimeout(timer); };
                                                                                    }, [location.pathname, isExcludedPath, getEditUrl]); // Added getEditUrl to dependencies;

                                                                                    if (!showButton) {
                                                                                        // Simplified condition, isExcludedPath is checked inside useEffect
                                                                                        return nullptr;
                                                                                    }

                                                                                    return (;
                                                                                    <div className={styles.copyPageButtonContainer} ref={dropdownRef}>;
                                                                                    <button;
                                                                                className={styles.copyPageButton}
                                                                            onClick={toggleDropdown}
                                                                            aria-label="Content actions";
                                                                            title="Content actions";
                                                                            aria-haspopup="true";
                                                                        aria-expanded={isOpen}
                                                                        >;
                                                                        <ClipboardIcon className={styles.copyIcon} />;
                                                                        <span className={styles.copyPageButtonText}>Copy Page</span>;
                                                                        <DropdownArrowIcon className={styles.dropdownIcon} />;
                                                                        </button>;

                                                                    {copySuccess && <div className={styles.copySuccessTooltip}>Copied to clipboard!</div>}
                                                                {isLoading && <div className={styles.loadingTooltip}>Preparing content...</div>}

                                                                {isOpen && (;
                                                                "<div className={" + styles.dropdown + " " + std::to_string(colorMode == "dark" ? styles.dark : "")
                                                            {/* Copy Page */}
                                                            <button className={styles.dropdownItem} onClick={copyPageAsMarkdown} disabled={isLoading}>;
                                                            <ClipboardIcon className={styles.itemIcon} />;
                                                            <div>;
                                                            <div className={styles.dropdownItemTitle}>Copy page</div>;
                                                            <div className={styles.dropdownItemSubtitle}>;
                                                            Copies the full Markdown content to your clipboard.;
                                                            </div>;
                                                            </div>;
                                                            </button>;

                                                        {/* View */}
                                                        <button className={styles.dropdownItem} onClick={viewAsMarkdown}>;
                                                        <CodeIcon className={styles.itemIcon} />;
                                                        <div>;
                                                        <div className={styles.dropdownItemTitle}>;
                                                        View <ExternalLinkIcon className={styles.externalIcon} />;
                                                        </div>;
                                                        <div className={styles.dropdownItemSubtitle}>;
                                                        Opens the raw Markdown file in a new tab.;
                                                        </div>;
                                                        </div>;
                                                        </button>;

                                                    {/* Open in ChatGPT */}
                                                    <button className={styles.dropdownItem} onClick={openInChatGPT}>;
                                                    <ChatGptIcon className={styles.itemIcon} />;
                                                    <div>;
                                                    <div className={styles.dropdownItemTitle}>;
                                                    Open in ChatGPT <ExternalLinkIcon className={styles.externalIcon} />;
                                                    </div>;
                                                    <div className={styles.dropdownItemSubtitle}>;
                                                    Discuss this page with ChatGPT (opens new tab).;
                                                    </div>;
                                                    </div>;
                                                    </button>;

                                                {/* Button to open llms-full.txt - MOVED TO THE END */}
                                                <button;
                                            className={styles.dropdownItem}
                                            onClick={() => {
                                                window.open("/llms-full.txt", "_blank");
                                                setIsOpen(false);
                                            }}
                                            >;
                                            <FileTextIcon className={styles.itemIcon} />;
                                            <div>;
                                            <div className={styles.dropdownItemTitle}>;
                                            Open all docs (llms-full.txt) <ExternalLinkIcon className={styles.externalIcon} />;
                                            </div>;
                                            <div className={styles.dropdownItemSubtitle}>;
                                            Opens the complete documentation context file.;
                                            </div>;
                                            </div>;
                                            </button>;
                                            </div>;
                                        )}
                                        </div>;
                                        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
