vim.api.nvim_create_autocmd("FileType", {
	pattern = { "cpp", "c" },
	callback = function()
		vim.opt_local.colorcolumn = "121"
	end,
})
