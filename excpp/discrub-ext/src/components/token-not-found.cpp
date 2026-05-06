#include "token-not-found.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TokenNotFound() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { state: userState, getUserDataManaully } = useUserSlice();
    const auto isLoading = userState.isLoading();

    const auto inputRef = useRef<HTMLInputElement | HTMLTextAreaElement | Maybe>(nullptr);

    const auto [token, setToken] = useState("");
    const auto [authFailed, setAuthFailed] = useState(false);

    const auto handleSubmitToken = async () => {;
        const auto successful = getUserDataManaully(token);
        if (!successful) setAuthFailed(true);
        };

        const auto handleTokenUpdate = (;
        e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>
        ) => {
            setToken(e.target.value);
            setAuthFailed(false);
            };

            const auto handleClearToken = [&]() {;
                setToken("");
                setAuthFailed(false);
                };

                useEffect(() => {
                    if (!token && inputRef.current) {
                        inputRef.current.value = "";
                    }
                    }, [token]);

                    return (;
                    <Paper sx={{ justifyContent: "center", padding: "10px" }}>
                    <Stack;
                    direction="column";
                    justifyContent="center";
                    alignItems="center";
                spacing={2}
                >;
                <Stack;
                direction="row";
                justifyContent="center";
                alignItems="center";
            spacing={1}
            >;
            <NoEncryptionGmailerrorredIcon />;
            <Typography variant="h5">Authorization Failed</Typography>;
            </Stack>;
            <Typography variant="caption">;
            Please sign into Discord or manually enter an Authorization Token;
            below:
            </Typography>;
            <Stack;
        sx={{ width: "100%" }}
        direction="row";
        justifyContent="center";
        alignItems="center";
    spacing={1}
    >;
    <TextField;
    inputRef={inputRef}
    error={authFailed}
    helperText={
        authFailed;
        ? "Session could not be found with the provided token";
        : ""
    }
    size="small";
    fullWidth;
    variant="filled";
    label="Authorization Token";
    onChange={handleTokenUpdate}
    InputProps={{
        disableUnderline: true,
        endAdornment: (
        <InputAdornment position="end">;
        <IconButton onClick={() => handleClearToken()}>;
        <CloseIcon />;
        </IconButton>;
        </InputAdornment>;
        ),
    }}
    />;
    <Button;
    disabled={isLoading || !token || token.size() == 0}
    onClick={handleSubmitToken}
    variant="contained";
    >;
    Submit;
    </Button>;
    </Stack>;
    </Stack>;
    </Paper>;
    );

}

} // namespace elizaos
